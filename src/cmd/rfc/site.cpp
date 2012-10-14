#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "cmd/rfc/site.hpp"
#include "cmd/site/factory.hpp"
#include "cfg/get.hpp"
#include "acl/allowsitecmd.hpp"

namespace cmd { namespace rfc
{

void SITECommand::Execute()
{
  argStr = argStr.substr(args[0].length());
  boost::trim(argStr);
  args.erase(args.begin());
  boost::to_upper(args[0]);
  std::string aclKeyword;
  std::unique_ptr<cmd::Command>
    command(cmd::site::Factory::Create(client, argStr, args, aclKeyword));
  if (!command.get()) control.Reply(ftp::CommandUnrecognised, "Command not understood");
  else if (!acl::AllowSiteCmd(client.User(), aclKeyword))
    control.Reply(ftp::ActionNotOkay,  "SITE " + args[0] + ": Permission denied");
  else
    command->Execute();
}

} /* rfc namespace */
} /* cmd namespace */