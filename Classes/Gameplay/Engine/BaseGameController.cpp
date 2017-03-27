//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Gameplay/Engine/BaseGameController.h"
#include "Gameplay/Engine/GameLogic.h"
#include "Gameplay/Engine/GameObject/GameScene.h"

namespace Echo
{

    BaseGameController::BaseGameController()
        : m_pCurrentScene(NULL)
        , m_pUiController(NULL)
    {}

    bool BaseGameController::init(String controllerName)
    {
        m_controllerName = controllerName;

        // register to game logic
        GameLogic::getInstance()->registerGameController(this);

        return true;
    }

    bool BaseGameController::uninit()
    {
        GameLogic::getInstance()->unregisterGameController(this);

        return true;
    }

    String BaseGameController::getSceneName()
    {
        ECHO_ASSERT(m_pCurrentScene, "m_pCurrentScene has not been assigned!");
        return m_pCurrentScene->getSceneName();
    }

    String BaseGameController::getControllerName()
    {
        return m_controllerName;
    }

    UIController* BaseGameController::getUIController()
    {
        if (!m_pUiController)
        {
            m_pUiController = m_pCurrentScene->getUiController();
        }

        return m_pUiController;
    }

    void BaseGameController::setOwner(GameScene* pOwner)
    {
        m_pCurrentScene = pOwner;
    }

}