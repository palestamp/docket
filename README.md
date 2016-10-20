


## Development

You will need CMake for developing

```
git clone https://palestamp@bitbucket.org/palestamp/docket.git
cd docket
mkdir build
cd build
cmake ..
make
```

Then you can try from build directiry:
```
./src/docket add ../local.docket
./src/docket list
./src/docket tree
```

## Usage

`docket add localex.docket`

`docket tree`

```
development
* @@ Order (176)

parts
+----paths
|    * @@ Overview (140)
|    * @@ Details (193)
|
+----todo
|    +---config
|    |   * @@ use-config flag (153)
|    |   * @@ Namespaces (768)
|    |
|    +---layouts
|    |   * @@ Overview (418)
|    |
|    +---timers
|        * @@ Overview (861)
|
+----done
     +---paths
         +----globbing
              * @@ Overview (307)

tech
+---parts
    +----body
         * @@ Parse on request (40)
```
