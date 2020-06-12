#include "usernode.h"
#include <quasarapp.h>
#include <userrequest.h>
#include <badrequest.h>
#include "sqldbcache.h"
#include "usernodeinfo.h"

namespace NP {

UserNode::UserNode()
{

}

ParserResult UserNode::parsePackage(const Package &pkg,
                                    const WP<AbstractNodeInfo> &sender) {

    auto parentResult = BaseNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    auto strongSender = sender.toStrongRef();

    if (H_16<UserRequest>() == pkg.hdr.command) {
        auto cmd = SP<UserRequest>::create(pkg);

        if (!cmd->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithUserRequest(cmd, strongSender->id(), &pkg.hdr)) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }
        return ParserResult::Processed;


    } else if (H_16<UserBaseData>() == pkg.hdr.command)  {
        auto cmd = SP<UserBaseData>::create(pkg);

        if (!cmd->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }



        return ParserResult::Processed;
    }

    return ParserResult::NotProcessed;

}

QVariantMap UserNode::defaultDbParams() const {
    return BaseNode::defaultDbParams();
}




}
