# lockgl

An OpenGL lockscreen for X11

![Example](example.gif)

## Usage

Lock the display once:

```sh
lockgl
```

Optionally, use a utility like [`xidlehook`](https://gitlab.com/jD91mZM2/xidlehook):

```sh
xidlehook --timer 300 lockgl "" &
```

## Installation

Build and install `lockgl` with the following command:

```sh
make install
```

### Requirements

Before building `lockgl`, the following package requirements must be met:

- `glew`
- `libx11`
- `xorgproto`

## Acknowledgements

- [`gllock`](https://github.com/kuravih/gllock) for the original inspiration
- [Efficient Gaussian Blur with Linear Sampling](https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/) for the technical breakdown of a Gaussian Filter
