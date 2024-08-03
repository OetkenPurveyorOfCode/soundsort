# Soundsort

## Building

```
cc main.c -o main.exe
```

## Example

Have a two channel wave file ready (here in.wav)

```
main reverse mergesort in.wav out.wav 1
```

"enjoy" your out.wav file.

### PS

If you don't have a two channel wave file you can create one like so:
```
ffmpeg -i song.mp3 -ac 2 song.wav
```

# Usage

```
Usage: main <mode> <sort> <infile> outfile> <chunksize>
```
