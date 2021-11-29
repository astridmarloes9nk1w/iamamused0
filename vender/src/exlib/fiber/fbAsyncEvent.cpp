/*
 *  fbAsyncEvent.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <exlib/fiber.h>

namespace exlib
{

AsyncEvent::AsyncEvent()
{
    m_next = NULL;
    m_service = Service::getTLSService();
}

void AsyncEvent::post()
{
    m_service->m_aEvents.put(this);
}

}
