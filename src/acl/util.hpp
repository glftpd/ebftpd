#ifndef __ACL_UTIL_HPP
#define __ACL_UTIL_HPP

#include <string>

namespace acl
{

void CreateDefaults();

enum class ValidationType
{
  Username,
  Groupname,
  Tagline
};

bool Validate(ValidationType type, const std::string& s);

class UserProfile;

std::string RatioString(const UserProfile& profile);
std::string CreditString(const UserProfile& profile);

} /* acl namespace */

#endif