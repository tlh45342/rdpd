"""Run: python -m tests.rdpclient --host 127.0.0.1 --output desktop.png"""
import argparse
from . import RDPClient

def main():
    parser = argparse.ArgumentParser(description="RDPD synthetic protocol screenshot test")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=3389)
    parser.add_argument("--output", default="desktop.png")
    parser.add_argument("--click", nargs=2, type=int, metavar=("X","Y"))
    args = parser.parse_args()
    with RDPClient(args.host,args.port) as client:
        print("PASS: X.224 legacy negotiation")
        client.activate()
        print("PASS: MCS and RDP activation")
        print("PASS: framebuffer %dx%d" % (client.width,client.height))
        if args.click:
            client.click(*args.click)
            print("PASS: mouse click and framebuffer update")
        client.save_png(args.output)
        print("PASS: screenshot saved to",args.output)

if __name__ == "__main__":
    main()
