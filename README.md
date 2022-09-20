# png-explorer

A C based CLI tool to interact with png images - display png info, find png's and concatenate multiple pngs into a single png 

## Instlallation

```
make all
```

## Usage

### `findpng`

Finds png files under a particular directory
```shell
findpng <DIRECTORY>
```

### `pnginfo`

Lists png dimensions and checks if png is valid
```shell
pnginfo <FILE>
```

### `catpng`

Concatenates multiple pngs into one
```shell
catpng <FILE1> <FILE2> ...
```
