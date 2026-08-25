# 🚀 **ASCII RENDERER** 🚀
# 3D ASCII Renderer

An ASCII Renderer I made over a weekend to end this summer! Loads of fun and very rewarding! All one C file.

It just draws a torus and lines using precomputed matrix math, and uses the dot product of the light position and surface normal to get the angle and thereby brightness level. Heavy inspiration from [@a1k0n](https://github.com/a1k0n) and [his explanation](https://www.a1k0n.net/2011/07/20/donut-math.html) of his donut.c code.

Also this is my first time using markdown so I hope you like it lol

![example gif](./ASCII_RENDERER_GIF.gif)

## 🛠️ Building & Running

```bash
$ git clone https://github.com/TweetyGuy/ascii-renderer
$ cd ascii-renderer
$ make test
```

(Requires gcc)

## 🔌 Controls

### 🎥 Camera Movement
 - Space    — Pause/Unpause Automatic Camera Movement

 - W        — Camera Up
 - A        — Camera Left
 - S        — Camera Down
 - D        — Camera Right

 - \-/_     — Zoom Out
 - +/=      — Zoom In

### 🔢 Parameter Modification
Each key followed by a number 0-9
 - R        — Modify Torus Thickness
 - T        — Modify Torus Cavity Size
 - L        — Modify XYZ Coordinate Axes Length (gets multiplied by 2)

### 💡 Light Source Movement
On the global z = -1 plane
 - Arrow Up — Move light up
 - Arrow Left — Move light left
 - Arrow Down — Move light down
 - Arrow Right — Move light right

### ❓ Miscellaneous
 - q        — Quit

## ⏳ Future Plans
Would love to add more stuff like a cube and/or pyramid! Those plans seem like I would have to modify my current (very rudimentary) implementation of the torus quite a bit however.

## ⚖️ License

MIT License