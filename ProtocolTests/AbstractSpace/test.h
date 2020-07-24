#ifndef TEST_H
#define TEST_H

#define TEST_LOCAL_HOST "127.0.0.1"
#define TEST_PORT 27777

class Test
{
public:
    Test() = default;
    virtual ~Test() = default;
    virtual void test() = 0;
};

#endif // TEST_H
