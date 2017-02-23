# Space Invaders

A Space Invaders emulator, written in C with OpenGL (and GLFW).

![space invaders demo](demo.gif)

To run it: `make && ./invaders`. Keys are: `c` to insert a coin, `enter` to start a game, `left` / `right` to move, and `space` to shoot.

To run the tests, run with `--test`. The emulator passes the following tests:
- [x] TST8080.COM
- [x] 8080PRE.COM
- [ ] 8080EX1.COM
- [ ] CPUTEST.COM

# To do

- 2 player game
- fix CPU to pass remaining tests
- add sound
- big-endian systems?

# Resources used

- [CPU instructions](http://nemesis.lonestar.org/computers/tandy/software/apps/m4/qd/opcodes.html) and [these](http://www.pastraiser.com/cpu/i8080/i8080_opcodes.html)
- [thibaultimbert's Intel8080](https://github.com/thibaultimbert/Intel8080/blob/master/8080.js) and [begoon's i8080-js](https://github.com/begoon/i8080-js)
