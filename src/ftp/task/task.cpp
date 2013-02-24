#include <sstream>
#include "ftp/task/task.hpp"
#include "ftp/server.hpp"
#include "logs/logs.hpp"
#include "cfg/get.hpp"
#include "main.hpp"
#include "text/factory.hpp"
#include "text/error.hpp"
#include "cfg/error.hpp"
#include "ftp/data.hpp"
#include "ftp/client.hpp"

namespace ftp { namespace task
{

void Task::Push()
{
  ftp::Server::PushTask(shared_from_this());
}

void KickUser::Execute(Server& server)
{
  int kicked = 0;
  for (auto& client: server.clients)
  {
    if (client.User().ID() == uid)
    {
      client.Interrupt();
      ++kicked;
      if (oneOnly) break;
    }
  }
  
  promise.set_value(kicked);
}

void LoginKickUser::Execute(Server& server)
{
  Result result;
  for (auto& client: server.clients)
  {
    if (client.User().ID() == uid && client.State() == ftp::ClientState::LoggedIn)
    {
      if (!result.kicked)
      {
        client.Interrupt();
        result.kicked = true;
        result.idleTime = client.IdleTime();
      }
      
      ++result.logins;
    }
  }
  
  promise.set_value(result);
}

void GetOnlineUsers::Execute(Server& server)
{
  for (auto& client: server.clients)
  {
    if (client.State() != ClientState::LoggedIn) continue;
    users.emplace_back(client.User().ID(), client.Data().State(), client.IdleTime(), 
                       client.CurrentCommand(), client.Ident(), client.Hostname());
  }
  
  promise.set_value(true);
}

void ReloadConfig::Execute(Server&)
{
  Result configResult = Result::Okay;
  try
  {
    cfg::UpdateShared(cfg::Config::Load());
  }
  catch (const cfg::ConfigError& e)
  {
    logs::Error("Failed to load config: %1%", e.Message());
    configResult = Result::Fail;
  }
  
  Result templatesResult = Result::Okay;
  
  try
  {
    text::Factory::Initialize();
  }
  catch (const text::TemplateError& e)
  {
    logs::Error("Templates failed to initialise: %1%", e.Message());
    templatesResult = Result::Fail;
  }

  if (cfg::RequireStopStart()) configResult = Result::StopStart;
  promise.set_value(std::make_pair(configResult, templatesResult));
}

void Exit::Execute(Server&)
{
  ftp::Server::SetShutdown();
}

void UserUpdate::Execute(Server& server)
{
  for (auto& client: server.clients)
  {
    if (client.State() == ClientState::LoggedIn && client.User().ID() == uid)
    {
      client.SetUserUpdated();
    }
  }
}

void OnlineUserCount::Execute(Server& server)
{
  for (const auto& client: server.clients)
  {
    if (client.State() != ClientState::LoggedIn) continue;
    ++count;
    ++allCount;
  }
  
  promise.set_value();
}

}
}
