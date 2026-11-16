"""Export embedded RGB565 icons as PNG and multi-resolution ICO resources."""
from pathlib import Path
import re
from PIL import Image
resource=Path(__file__).resolve().parents[1]/"resources"/"icons"
source=(resource/"mac_icon_assets.inc").read_text()
for name in ("disk","trash","calculator"):
    match=re.search(r"static const icon_resource mac_"+name+r" = \{\s*\{([^}]+)\},\s*\{(.*?)\}\s*\};",source,re.S)
    if not match: raise ValueError(name)
    palette=[int(v,16) for v in re.findall(r"0x[0-9a-fA-F]+",match.group(1))]
    rows=re.findall(r'"([0-9a-f]{48})"',match.group(2))
    assert len(palette)==16 and len(rows)==48
    pixels=[]
    for row in rows:
        for char in row:
            index=int(char,16)
            if not index: pixels.append((0,0,0,0));continue
            value=palette[index]
            r=(value>>11)&31;g=(value>>5)&63;b=value&31
            pixels.append((r*255//31,g*255//63,b*255//31,255))
    image=Image.new("RGBA",(48,48))
    image.putdata(pixels)
    image.save(resource/(name+".png"))
    image.save(resource/(name+".ico"),format="ICO",sizes=[(16,16),(24,24),(32,32),(48,48)])
    print("generated",name)
