#ifndef RATINGTABLEWRITER_H
#define RATINGTABLEWRITER_H

#include "asyncsqldbwriter.h"

namespace NetworkProtocol {


class RatingTableWriter : public AsyncSqlDbWriter
{
    Q_OBJECT
public:
    RatingTableWriter();
private:

};

}

#endif // RATINGTABLEWRITER_H
