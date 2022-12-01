# QuasarApp Heart

# ![QuasarApp Heart](res/Banner_Web.png)

QuasarApp Heart - it is base backend for C++/Qt projects. This library support work with databases and work with lite network requests.

## Futures 
This library consists of two levels (AbstractNode level and DataBaseNode level).

- [X] Support ssl sockets
- [X] Support initialize database
- [X] Support work in database


## Build and Include
### For cmake projects
 
 * cd yourRepo
 * git submodule add https://github.com/QuasarApp/Heart.git # add the repository of Heart into your repo like submodule
 * git submodule update --init --recursive
 * Include in your CMakeLists.txt file the main CMakeLists.txt file of Heart library
 
     ```cmake
     add_subdirectory(Heart)
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

    // override this method for validation your package class
    bool isValid() const {
        return AbstractData::isValid();
    };                     

    // your data for for server of client
    std::string _data = "";

protected:
    // StreamBase interface override this methods for serialization your package
    QDataStream &fromStream(QDataStream &stream) {
        stream >> _data;
        return stream;
    }
    QDataStream &toStream(QDataStream &stream) const {
        stream << _data;
        return stream;
    }

};
```

Create your parser api class. 

```cpp
*
* class MyParser: public QH::iParser {
   public:
       MyParser(QH::AbstractNode* parentNode): QH::iParser(parentNode) {
           registerPackageType<MyPackage>();
           data = new BigPackage();
   }

   // iParser interface
   public:

   // override this method for processed received data.
   ParserResult parsePackage(const Package &pkg,
                             const AbstractNodeInfo *sender) {

       auto parentResult = AbstractNode::parsePackage(pkg, sender);
       if (parentResult != ParserResult::NotProcessed) {
           return parentResult;
       }

       auto result = commandHandler<MyPackage>(this, &MyClass::processMyPackage, pkg, sender, pkgHeader);
       if (result != QH::ParserResult::NotProcessed) {
           return result;
       }

       return ParserResult::NotProcessed;
   }

   bool processMyPackage(const QSharedPointer<MyPackage> &cardrequest,
                         const QH::AbstractNodeInfo *sender, const QH::Header &hdr) {

       BaseId requesterId = getSender(sender, &cardrequest);

       if (!cardrequest.isValid()) {
           badRequest(sender->networkAddress(), hdr);
           return ParserResult::Error;
       }

       cardrequest._data = "responce for client "

       // responce only for servers.
       if (nodeType() == QH::AbstractNode::NodeType::Server)
           sendData(cardrequest, sender->networkAddress(), &pkg.hdr);

       return ParserResult::Processed;
   }

   // This vesion of the parser (any digital value.) .
   int version() const override {return 0;};
   QString parserId() const override {return "MyParser";};

   };
```

Create a server class and add supported parsers to work with packages.
```cpp
class TestingServer: public QH::AbstractNode {
Q_OBJECT
public:
    TestingServer() {
        addApiParser<MyParser>();
    }

    QH::AbstractNode::NodeType nodeType() const override {
        return QH::AbstractNode::NodeType::Server;
    }
};
```

Create a client class and add supported parsers to work with packages.

```cpp

class TestingClient: public QH::AbstractNode {

public:
    TestingClient() {
        addApiParser<MyParser>();
    }

    QH::AbstractNode::NodeType nodeType() const override {
        return QH::AbstractNode::NodeType::Client;
    }
};
```
For more information, see our QuasarApp Heart [Code](https://quasarapp.ddns.net:3031/docs/QuasarApp/Heart/latest/) Documentation.
