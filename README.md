# massif-cherrypick

A massif (valgrind) extension to analyze memory usage for only specific functions.

massif-cherrypick takes an massif.out file, and  
**forges** a new one containing only stats from specified functions that match the specified pattern.

The new massif.out file can then be used with `ms_print`, `massif-visualizer`.

## Dependencies

- g++
- ... and nothing else


## Building

```
make
```


## Usage

```
./massif-cherrypick {massif.out file} {pattern} [--merge-stacks] [--clear-heap-extra]
```

