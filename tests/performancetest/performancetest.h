/******************************************************************************
 *
 * package:     Log4Qt
 * file:        performancetest.h
 * created:     February 2026
 * author:      Performance Test
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef PERFORMANCETEST_H
#define PERFORMANCETEST_H

#include <QObject>
#include <QtTest>

class PerformanceTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Performance tests
    void testSimpleLoggingPerformance();
    void testSimpleLoggingPerformance_data();
    
    void testFileAppenderPerformance();
    void testFileAppenderPerformance_data();
    
    void testMultiThreadedLoggingPerformance();
    void testMultiThreadedLoggingPerformance_data();
    
    void testFormattingPerformance();
    void testFormattingPerformance_data();
    
    void testFilteringPerformance();
    void testFilteringPerformance_data();

private:
    QString mTestDir;
};

#endif // PERFORMANCETEST_H
