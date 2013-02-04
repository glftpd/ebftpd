#ifndef __ACL_USERCACHE_HPP
#define __ACL_USERCACHE_HPP

#include <utility>
#include <unordered_map>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "acl/user.hpp"
#include "util/error.hpp"
#include "acl/replicable.hpp"

namespace acl
{

class UserCache : public Replicable
{
  typedef std::unordered_map<std::string, acl::User*> ByNameMap;
  typedef std::unordered_map<UserID, acl::User*> ByUIDMap;
  typedef std::unordered_map<acl::UserID, std::vector<std::string>> IPMaskMap;
  
  mutable boost::mutex createMutex;
  mutable boost::mutex mutex;
  ByNameMap byName;
  ByUIDMap byUID;

  mutable boost::shared_mutex ipMutex;
  IPMaskMap ipMasks;
  
  boost::posix_time::ptime lastReplicate;
  
  static UserCache instance;
  static bool initialized;
  
  UserCache() : lastReplicate(boost::gregorian::date(1970, 1, 1)) { }
  
  ~UserCache();
  
  static void Save(const acl::User& user);
  static void Save(const acl::User& user, const std::string& field);
  
public:
  bool Replicate();

  static bool Exists(const std::string& name);
  static bool Exists(UserID uid);
  static util::Error Create(const std::string& name, const std::string& password,
                     const std::string& flags, const UserID creator);
  static util::Error Delete(const std::string& name);
  static util::Error Purge(const std::string& name);
  static util::Error Readd(const std::string& name);
  static util::Error Rename(const std::string& oldName, const std::string& newName);
  static util::Error SetPassword(const std::string& name, const std::string& password);
  static util::Error SetFlags(const std::string& name, const std::string& flags);
  static util::Error AddFlags(const std::string& name, const std::string& flags);
  static util::Error DelFlags(const std::string& name, const std::string& flags);
  static util::Error SetPrimaryGID(const std::string& name, GroupID primaryGID, GroupID oldPprimaryGID);
  static util::Error SetPrimaryGID(const std::string& name, GroupID primaryGID)
  {
    GroupID oldGID = -1;
    return SetPrimaryGID(name, primaryGID, oldGID);
  }
  static util::Error AddGID(const std::string& name, GroupID gid);
  static util::Error DelGID(const std::string& name, GroupID gid);
  static util::Error ResetGIDs(const std::string& name);
  static GroupID PrimaryGID(UserID uid);
  static bool HasGID(const std::string& name, acl::GroupID gid);

  static util::Error ToggleGadminGID(const std::string& name, GroupID gid, bool& added);
//  static util::Error DelGadminGID(const std::string& name, GroupID gid);
  
  static unsigned Count(bool includeDeleted = false);

  static void Initialize();
  
  // these return const as the user objects should NEVER
  // be modified except via the above functions'
  static acl::User User(const std::string& name);
  static acl::User User(UserID uid);
  
  static UserID NameToUID(const std::string& name);
  static std::string UIDToName(UserID uid);
  
  static bool IPAllowed(const std::string& address);
  static bool IdentIPAllowed(acl::UserID uid, const std::string& identAddress);
  
  static util::Error AddIPMask(const std::string& name, const std::string& mask,
                               std::vector<std::string>& redundant);
  static util::Error AddIPMask(const std::string& name, const std::string& mask);  
  static util::Error DelIPMask(const std::string& name, int index, std::string& deleted);
  static util::Error DelAllIPMasks(const std::string& name, std::vector<std::string>& deleted);
  
  static util::Error ListIPMasks(const std::string& name, std::vector<std::string>& masks);  
};

} /* acl namespace */

#endif
