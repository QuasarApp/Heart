# QuasarApp Heart
QuasarApp Heart - it is base backend for C++/Qt projects. This library support work with databases and work with lite network requests.

## Futures 
This library consists of two levels (AbstractNode level and DataBaseNode level).

- [X] Support ssl sockets
- [X] Support initialize database
- [X] Support work in database
- [ ] Support decentralized network mode

### AbstractNode level (0)
#### Description
The AbstractNode level implement only base functions of create a new work threads and parsing packages.
For more information see QuasarApp Heart documentation, QH namespace.

### DataBaseNode level (1)
#### Description
The DataBaseNode level implement methods and packages for work with databases. This level using Qt classes for wrking with database, so for more information about suport databases see [Qt Documentation](https://doc.qt.io/qt-5/sql-driver.html).

### NetworkNode level (2)
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

Create a own package class and override some basic methods.
```cpp

class MyPackage: public QH::AbstractData
{
public:
    MyPackage();

    // constructor for create object from package
    MyPackage(const Package& from): QH::AbstractData(from) {

    };

    // override this method for validation your package class
    bool isValid() const {
        return AbstractData::isValid();
    };                     /
    // override this method for copy object data from other to this object
    bool copyFrom(const AbstractData *other) {
        if (!AbstractData::copyFrom(other))
            return false;

        auto otherObject = dynamic_cast<const MyPackage*>(other);
        if (!otherObject)
            return false;

        this->_data = otherObject->_data;
        return true;
    };

    // your data for for server of client
    std::string _data = "";

protected:
    // StreamBase interface override this methods for serialization your package
    QDataStream &fromStream(QDataStream &stream) {
        AbstractData::fromStream(stream);
        stream >> _data;
        return stream;
    }
    QDataStream &toStream(QDataStream &stream) const {
        AbstractData::toStream(stream);
        stream << _data;
        return stream;
    }

};
```

Create a server class and override parsePackage method for work with packages.
```cpp
class TestingServer: public QH::AbstractNode {


protected:
    // override this method for processed received data.
    ParserResult parsePackage(const Package &pkg,
                                            const AbstractNodeInfo *sender) {
                                            
        auto parentResult = AbstractNode::parsePackage(pkg, sender);
        if (parentResult != ParserResult::NotProcessed) {
            return parentResult;
        }
    
        if (H_16<MyPackage>() == pkg.hdr.command) {
            MyPackage obj(pkg);
    
            BaseId requesterId = getSender(sender, &obj);
    
            if (!obj.isValid()) {
                badRequest(sender->networkAddress(), pkg.hdr);
                return ParserResult::Error;
            }
            
            obj._data = "responce for client "
            
            SendData(&obj, sender->networkAddress(), &pkg.hdr);
            return ParserResult::Processed;            
        }
        return ParserResult::NotProcessed;
    
    }
};
```

Create a client class and override parsePackage method for work with packages.

```cpp

class TestingClient: public QH::AbstractNode {


protected:
    // parsing incoming packages
    ParserResult parsePackage(const Package &pkg,
                                            const AbstractNodeInfo *sender) {
                                            
        auto parentResult = AbstractNode::parsePackage(pkg, sender);
        if (parentResult != ParserResult::NotProcessed) {
            return parentResult;
        }
    
        if (H_16<MyPackage>() == pkg.hdr.command) {
            MyPackage obj(pkg);
   
            // print responce of server
            std::cout << obj._data;
            ...
            return ParserResult::Processed;            
        }
        // Do not forget return status of parsing packages
        return ParserResult::NotProcessed;
    
    }
    // sending request to server
    bool sendMyPackage() {
        Ping cmd;
        return sendData(&cmd, address);
    }
};
```



