/**
 * @file plugin.cpp
 * @brief The main file of the plugin
 */

#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Block.hpp>
#include <llapi/mc/Container.hpp>
#include <llapi/mc/BlockActor.hpp>
#include <llapi/mc/ChestBlockActor.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/utils/StringHelper.h>
#include "version.h"

extern Logger logger;

void SendContentToPlayer(Player* pl, Container* container)
{
    auto slots = container->getAllSlots();
    std::string contentStr;
    int cnt = 0;
    for(auto item : slots)
    {
        if(!item->isNull())
        {
            ++cnt;
            string name = item->getName();
            if(name.size() > 50)
                name = name.substr(0, 50) + "...";     // prevent overflow attack
            contentStr += "§6" + name + "§2(" + std::to_string(item->getCount()) + ")§r, ";
        }
    }
    if(cnt == 0)
    {
        pl->sendText("[ContentReader] 点击的容器为空！");
    }
    else
    {
        pl->sendText("[ContentReader] 点击的容器中共有" + std::to_string(cnt)
            + "堆物品：\n" + contentStr.substr(0, contentStr.size() - 2));
    }
}

void PluginInit()
{
    Event::BlockInteractedEvent::subscribe([](Event::BlockInteractedEvent e) -> bool
    {
        if(e.mPlayer)
        {
            auto &bl = e.mBlockInstance;
            if(bl.hasContainer())
            {
                auto typeName = bl.getBlock()->getTypeName();
                if(typeName == "minecraft:chest" || EndsWith(typeName, "shulker_box"))
                {
                    // chest or shulker_box, check if can be open
                    auto blockActor = (ChestBlockActor*)bl.getBlockEntity();
                    if(!blockActor->canOpen(*(bl.getBlockSource())))
                    {
                        // cannot open, send content to player
                        SendContentToPlayer(e.mPlayer, bl.getContainer());
                    }
                }
            }
        }
        return true;
    });
    logger.info("ContainReader已加载。作者：yqs112358");
}
