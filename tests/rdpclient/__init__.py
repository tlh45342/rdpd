"""Minimal RDPD development client. Not a general-purpose Windows RDP client."""
import socket
import struct

LEGACY = bytes.fromhex("03 00 00 0b 06 e0 00 00 00 00 00")
CONFIRM = bytes.fromhex("03 00 00 0b 06 d0 00 00 00 00 00")
MCS = bytes.fromhex("03 00 00 8b 02 f0 80 7f 65 81 80") + bytes(128)


def exact(sock, size):
    result = bytearray()
    while len(result) < size:
        chunk = sock.recv(size - len(result))
        if not chunk:
            raise EOFError("connection closed while receiving RDP packet")
        result.extend(chunk)
    return bytes(result)


def tpkt(sock):
    header = exact(sock, 4)
    length = int.from_bytes(header[2:4], "big")
    if header[:2] != b"\x03\x00" or length < 7 or length > 65535:
        raise ValueError("invalid TPKT header")
    return header + exact(sock, length - 4)


def share_data(subtype, body):
    length = 18 + len(body)
    share = (length.to_bytes(2, "little") + bytes.fromhex("17 00 ea 03 ea 03 01 00")
             + bytes.fromhex("00 01") + length.to_bytes(2, "little")
             + bytes([subtype, 0, 0, 0]) + body)
    if len(share) > 127:
        raise ValueError("fixture supports short PER length only")
    mcs = bytes.fromhex("64 00 01 03 eb 70") + bytes([len(share)]) + share
    return b"\x03\x00" + (len(mcs)+7).to_bytes(2,"big") + bytes.fromhex("02 f0 80") + mcs


class RDPClient:
    """RDPD's current legacy-security synthetic protocol test client."""

    def __init__(self, host="127.0.0.1", port=3389, timeout=10):
        self.host, self.port, self.timeout = host, port, timeout
        self.sock = None
        self.width, self.height = 1024, 768
        self.pixels = bytearray(self.width * self.height * 4)

    def __enter__(self):
        return self.connect()

    def __exit__(self, *_):
        self.close()

    def connect(self):
        self.sock = socket.create_connection((self.host, self.port), self.timeout)
        self.sock.settimeout(self.timeout)
        self.sock.sendall(LEGACY)
        if tpkt(self.sock) != CONFIRM:
            raise ValueError("RDPD legacy negotiation failed (TLS/NLA not supported)")
        return self

    def send(self, data):
        self.sock.sendall(data)

    def activate(self, username="root", password="toor"):
        self.send(MCS)
        response = tpkt(self.sock)
        if response[7:9] != bytes.fromhex("7f 66"):
            raise ValueError("MCS Connect-Response expected")
        self.send(bytes.fromhex("03 00 00 0c 02 f0 80 04 01 00 01 00"))
        self.send(bytes.fromhex("03 00 00 08 02 f0 80 28"))
        if tpkt(self.sock) != bytes.fromhex("03 00 00 0b 02 f0 80 2e 00 00 01"):
            raise ValueError("AttachUserConfirm expected")
        for channel in ("ea", "eb"):
            self.send(bytes.fromhex("03 00 00 0c 02 f0 80 38 00 01 03 " + channel))
            reply = tpkt(self.sock)
            if reply[7] != 0x3e:
                raise ValueError("channel join rejected")
        fields = ["", username, password, "", ""]
        strings = [f.encode("utf-16le") for f in fields]
        info = (bytes(4) + (0x10).to_bytes(4, "little")
                + b"".join(len(s).to_bytes(2,"little") for s in strings)
                + b"".join(s + bytes(2) for s in strings))
        payload = bytes.fromhex("40 00 ff 30") + info
        if len(payload) > 320:
            raise ValueError("credentials too long for RDPD fixture")
        self.send(bytes.fromhex("03 00 01 4f 02 f0 80 64 00 01 03 eb 70 81 40")
                  + payload.ljust(320, b"\x00"))
        if tpkt(self.sock)[7] != 0x68 or tpkt(self.sock)[7] != 0x68:
            raise ValueError("licensing or Demand Active missing")
        confirm = bytes.fromhex("14 00 13 00 ea 03 ea 03 01 00") + bytes(10)
        mcs = bytes.fromhex("64 00 01 03 eb 70") + bytes([len(confirm)]) + confirm
        self.send(b"\x03\x00" + (len(mcs)+7).to_bytes(2,"big")
                  + bytes.fromhex("02 f0 80") + mcs)
        self.send(share_data(31, bytes.fromhex("01 00 ea 03")))
        self.send(share_data(20, bytes.fromhex("04 00 00 00 00 00 00 00")))
        self.send(share_data(20, bytes.fromhex("01 00 00 00 00 00 00 00")))
        self.send(share_data(39, bytes(8)))
        for subtype in (31,20,20,40):
            reply = tpkt(self.sock)
            if reply[7] != 0x68 or reply[28] != subtype:
                raise ValueError("server finalization mismatch")
        self.receive_frame()
        return self

    def receive_frame(self, count=384, sparse=False):
        # Initial activation is a full frame; later updates are sparse.
        old_timeout = self.sock.gettimeout()
        try:
            for i in range(count):
                if sparse:
                    self.sock.settimeout(min(old_timeout or 1.0, 0.35))
                try:
                    frame = tpkt(self.sock)
                except socket.timeout:
                    if sparse:
                        break
                    raise
                if frame[7] != 0x68 or not (frame[13] & 0x80):
                    raise ValueError("expected bitmap SendDataIndication")
                share = frame[15:]
                if share[14] != 2 or share[18:22] != bytes.fromhex("01 00 01 00"):
                    raise ValueError("expected bitmap update")
                left, top, right, bottom, width, height, bpp, flags, size = struct.unpack_from("<9H",share,22)
                if bpp != 32 or flags or size != width*height*4 or len(share[40:]) != size:
                    raise ValueError("unsupported bitmap format")
                if right != left+width-1 or bottom != top+height-1 or right >= self.width or bottom >= self.height:
                    raise ValueError("invalid bitmap bounds")
                data = share[40:]
                for row in range(height):
                    dest = ((top+height-1-row)*self.width + left)*4
                    self.pixels[dest:dest+width*4] = data[row*width*4:(row+1)*width*4]
        finally:
            self.sock.settimeout(old_timeout)
        return self

    def pixel(self, x, y):
        offset = (y*self.width+x)*4
        return int.from_bytes(self.pixels[offset:offset+4],"little")

    def click(self, x, y):
        event = (bytes(4) + (0x8001).to_bytes(2,"little")
                 + (0x9000).to_bytes(2,"little")
                 + x.to_bytes(2,"little") + y.to_bytes(2,"little"))
        self.send(share_data(28, b"\x01\x00\x00\x00" + event))
        self.receive_frame(sparse=True)
        return self

    def save_png(self, path):
        import zlib
        def chunk(kind, data):
            return struct.pack(">I",len(data))+kind+data+struct.pack(">I",zlib.crc32(kind+data)&0xffffffff)
        rgb = bytearray()
        for y in range(self.height):
            rgb.append(0)
            for x in range(self.width):
                value = self.pixel(x,y)
                rgb.extend(((value>>16)&255,(value>>8)&255,value&255))
        with open(path,"wb") as f:
            f.write(b"\x89PNG\\r\\n\x1a\\n")
            f.write(chunk(b"IHDR",struct.pack(">IIBBBBB",self.width,self.height,8,2,0,0,0)))
            f.write(chunk(b"IDAT",zlib.compress(bytes(rgb))))
            f.write(chunk(b"IEND",b""))

    def close(self):
        if self.sock:
            self.sock.close()
            self.sock = None
