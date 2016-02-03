# vgs-bgm-decoder
- decodes: `VGS's .BGM file` to `PCM`
- platform free

## How to use (draft)
- Create Context
- Load BGM data (from file or memory)
= Decode
- Unload BGM data
- Release Context

# API specification (draft)
## Create Context
#### prototyping
```
void* vgsdec_create_context();
```

#### return value
- `!NULL` : context
- `NULL` : error

## Release Context
#### prototyping
```
void vgsdec_release_context(void* context);
```

#### arguments
- `context` : context

#### return value
- `!NULL` : context
- `NULL` : error

## Load BGM data
#### prototyping
```
int vgsdec_load_bgm_from_file(void* context, int slot, const char* path);
int vgsdec_load_bgm_from_memory(void* context, int slot, void* data, size_t size);
```

#### arguments
- `context` : context
- `slot` : number of slot ( `0` ~ `255` )
- `path` : path of the BGM file
- `data` : pointer of memory area that BGM file data stored.
- `size` : size of the BGM file

#### return value
- `0` : success
- `-1` : failed

#### Remarks
- If you specified a loaded slot number already, these functions unload automatically old data and load new data.

## Unload BGM data
#### prototyping
```
void vgsdec_unload(void* context, int slot);
```

#### arguments
- `context` : context
- `slot` : number of slot ( `0` ~ `255` )
