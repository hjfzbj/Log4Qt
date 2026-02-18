# Performance Test Quick Start Guide

## Run the Test

### Option 1: Run Directly
```powershell
cd C:\Workspace\log4qt\build\bin\Release
.\performancetest.exe -v2
```

### Option 2: Run via CTest
```powershell
cd C:\Workspace\log4qt\build
ctest -C Release -R performancetest --output-on-failure
```

## Common Command Line Options

```bash
# Verbose output with benchmark results
.\performancetest.exe -v2

# Run only specific test
.\performancetest.exe testSimpleLoggingPerformance

# Run specific test with specific data
.\performancetest.exe testSimpleLoggingPerformance:"1000 DEBUG messages"

# List all available tests
.\performancetest.exe -functions

# List all data tags
.\performancetest.exe -datatags

# Output to XML file (for CI integration)
.\performancetest.exe -o results.xml -xunitxml
```

## Quick Performance Metrics

### Throughput (messages per second)
- **Simple logging**: ~2,500 msg/s
- **File (buffered)**: ~1,400 msg/s
- **File (immediate flush)**: ~370 msg/s
- **Multi-threaded (4 threads)**: ~588 msg/s

### Key Insights
- 💡 Buffered writing is **3-4x faster** than immediate flush
- 💡 Simple layout is **2x faster** than pattern layout
- 💡 Pattern complexity has **minimal impact**
- 💡 Filters have **negligible overhead**
- 💡 Multi-threading scales **linearly**

## Interpreting Results

### QBENCHMARK Output Format
```
RESULT : TestName():"DataTag":
     X.X msecs per iteration (total: YY, iterations: ZZ)
```

- **msecs per iteration**: Average time for one benchmark iteration
- **total**: Total time in milliseconds
- **iterations**: Number of iterations run

### Performance Baseline

If you see significantly different results, consider:
- System load (close other applications)
- Disk speed (SSD vs HDD makes a big difference)
- Antivirus interference (may slow file I/O)
- Debug vs Release build (always use Release for benchmarking)

## Troubleshooting

### Test doesn't run
- Ensure Qt DLLs are in PATH: `C:\Qt\6.11.0\msvc2022_64\bin`
- Verify log4qt.dll is in the same directory as performancetest.exe

### Slow performance
- Check if running Debug build (should be Release)
- Close other applications
- Check disk I/O (Task Manager > Performance > Disk)

### Missing output
- Use `-v2` flag for verbose output
- Redirect to file: `.\performancetest.exe -v2 > results.txt 2>&1`

## Extending the Tests

### Add New Test Case

1. Add data row in `_data()` method:
```cpp
void PerformanceTest::testYourTest_data()
{
    QTest::addColumn<int>("param");
    QTest::newRow("description") << 1000;
}
```

2. Implement test:
```cpp
void PerformanceTest::testYourTest()
{
    QFETCH(int, param);
    
    QBENCHMARK
    {
        // Code to benchmark
    }
}
```

3. Declare in header:
```cpp
private slots:
    void testYourTest();
    void testYourTest_data();
```

## CI/CD Integration

### Run in CI Pipeline

```yaml
# Example for GitHub Actions
- name: Run Performance Tests
  run: |
    cd build
    ctest -C Release -R performancetest --output-on-failure
```

### Generate XML Report
```bash
.\performancetest.exe -o performance_results.xml -xunitxml
```

## Support

For issues or questions:
1. Check the full README.md in this directory
2. Review PERFORMANCE_TEST_SUMMARY.md in project root
3. Examine the test source code for implementation details
