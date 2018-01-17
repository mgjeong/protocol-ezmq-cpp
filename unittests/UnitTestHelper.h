/*******************************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/

#ifndef UNITTESTHELPER_H
#define UNITTESTHELPER_H

#include <gtest/gtest.h>
#include <HippoMocks/hippomocks.h>

#include "Event.pb.h"

class TestWithMock: public testing::Test
{
public:
    MockRepository mocks;

protected:
    virtual ~TestWithMock() noexcept(noexcept(std::declval<Test>().~Test())) {}

    virtual void TearDown() {
        try
        {
            mocks.VerifyAll();
        }
        catch (...)
        {
            mocks.reset();
            throw;
        }
    }
};

ezmq::Event getProtoBufEvent()
{
    ezmq::Event event;
    event.set_device("device");
    event.set_created(10);
    event.set_modified(20);
    event.set_id("id");
    event.set_pushed(10);
    event.set_origin(20);

    ezmq::Reading *reading1 = event.add_reading();
    reading1->set_name("reading1");
    reading1->set_value("10");
    reading1->set_created(25);
    reading1->set_device("device");
    reading1->set_modified(20);
    reading1->set_id("id1");
    reading1->set_origin(25);
    reading1->set_pushed(1);

    ezmq::Reading *readin2  = event.add_reading();
    readin2->set_name("reading2");
    readin2->set_value("20");
    readin2->set_created(30);
    readin2->set_device("device");
    readin2->set_modified(20);
    readin2->set_id("id2");
    readin2->set_origin(25);
    readin2->set_pushed(1);

    return event;
}

#endif // UNITTESTHELPER_H

