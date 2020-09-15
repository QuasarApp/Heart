# QuasarApp Heart
QuasarApp Heart - it is base back for C++/Qt projects. 

- [X] Support ssl sockets
- [X] Support initialize database
- [X] Support work in database
- [ ] Sopport decentralized network mode

## Futures 
This library consists of two levels (AbstractNode level and DataBaseNode level).

### AbstractNode level (1)
#### Description
The AbstractNode level implement only base functons of create new work threads and parsing packages.
Example of use AbstractNode level

```cpp
EXAMPLE
```

For more information see QuasarApp Heart documentation.

### DataBaseNode level (2)
#### Description
The DataBaseNode level implement methods and packages for work with databases. This level using Qt classes for wrking with database, so for more information about suport databases see [Qt Documentation](https://doc.qt.io/qt-5/sql-driver.html).

Example of use DataBaseNode level

```cpp
EXAMPLE
```
### NetworkNode level (3)
#### Description
This level is still in develop. 

## Build and Include

### For cmake projects
 
 * cd yourRepo
 * git submodule add https://github.com/QuasarApp/Heart.git # add the repository of Heart into your repo like submodule
 * git submodule update --init --recursive
 * Include in your CMakeLists.txt file the main CMakeLists.txt file of Heart library
     ``` cmake
     add_subdirectory(Heart)
     ```
 * select requiriment build level for you project
     ```
     set(HEART_BUILD_LVL 2)
     ```
     where 1 - is code of build level 
   
     1 - AbstractNode
   
     2 - DataBaseNode
   
     3 - NetworkNode
 * link the Heart library to your target
     ```cmake
     target_link_libraries(yourLib PUBLIC Heart)
     ```
 * rebuild yuor project



## Usage

To-do



