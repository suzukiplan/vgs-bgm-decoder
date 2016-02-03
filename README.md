# VGS BGM Decoder
- decodes: `VGS's .BGM file` to `PCM`
- platform free

## How to use (draft)
- Create Context
- Load BGM data (from file or memory)
- Decode
- Release Context

## Create Context
#### prototyping
```
void* vgsdec_create_context();
```

#### return value
- `!NULL` : context
- `NULL` : error

## Load BGM data
#### prototyping
```
int vgsdec_load_bgm_from_file(void* context, const char* path);
int vgsdec_load_bgm_from_memory(void* context, void* data, size_t size);
```

#### arguments
- `context` : context
- `path` : path of the BGM file
- `data` : pointer of memory area that BGM file data stored.
- `size` : size of the BGM file

#### return value
- `0` : success
- `-1` : failed

## Decode
#### prototyping
```
void vgsdec_execute(void* context, void* buffer, size_t size);
```

#### arguments
- `context` : context
- `buffer` : stores decoded PCM data
- `size` : size of buffer

#### remarks
- this function stores decoded PCM data to the buffer.
- The format of the PCM are fixed by the following:
  - sampling rate: 22050Hz
  - bit rate: 16bit
  - channels: 1 (monoral)
- ex: 1 second = 44100 byte _(22050 * 16 * 1 / 8)_
- use to sequentially call

## Release Context
#### prototyping
```
void vgsdec_release_context(void* context);
```

#### arguments
- `context` : context
