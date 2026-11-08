# WORLD
- [ ] multiple chunks
    - [X] decide hashmap or simple 3d table of chunks : -> hashmap
    - [X] provide 1 big buffer for all chunk's meshes
- [X] save world
- [X] load world

# BLOCK
- [X] block state
    - [X] rotate block
## TEXTURE
- [X] check up&bot uv
- [X] give to gpu faces instead of vertices
- [X] put uvs directly in shaders
- [ ] make basic system for texture variants
    - variants :
        - [X] basic
        - [ ] animated
        - [X] activated
        - [ ] connected
        - [X] variant
        - [ ] random_rotation
    - [X] refractor the builder to do an array of node, dont use pointer for AST, use id of node's array
        - [X] memoize the face_texture_build() function
- [X] in the mesher: pick correct texture depending on block orientation
- [ ] orient face texture ( rotate in shader? or bake rotated texture ?)

# INTERFACE
- [ ] cube overlay cursor
- [x] proper input interface
- [ ] interact with world : place and remove blocks
    - [X] raytrace in voxel space
    - [ ] provide a way to select block : a BSM block selection menu
        - [X] simple 0 through 9 block selection
