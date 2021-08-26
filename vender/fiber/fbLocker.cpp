/*
 *  fbLocker.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <fiber.h>

namespace fiber
{

void Locker::lock()
{
    Service* pService = Service::getTLSService();
    Fiber* current = pService->m_running;

    if(m_locked && current != m_locker)
    {
        m_blocks.push_back(current);
        pService->switchto(&pService->m_main, FB_SUSPEND);
    }
    else if(++ m_count == 1)
    {
        m_locked = true;
        m_locker = current;
    }
}

bool Locker::trylock()
{
    Service* pService = Service::getTLSService();
    Fiber* current = pService->m_running;

    if(m_locked && current != m_locker)
        return false;

    if(++ m_count == 1)
    {
        m_locked = true;
        m_locker = current;
    }

    return true;
}

void Locker::unlock()
{
    Service* pService = Service::getTLSService();
    Fiber* current = pService->m_running;

    if(current == m_locker)
    {
        if(-- m_count == 0)
        {
            if(m_blocks.empty())
            {
                m_locked = false;
                m_locker = NULL;
            }
            else
            {
                m_count ++;
                m_locker = m_blocks.front();
                m_blocks.pop_front();
                pService->m_resume.push_back(m_locker);
            }
        }
    }
}

}
