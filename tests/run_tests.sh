#!/bin/bash
set -e

GWBASIC="./bin/gwbasic"
FAILED=0

echo "Running GW-BASIC Test Suite..."
echo "----------------------------------------"

run_test() {
    local name="$1"
    local file="$2"
    local expected_code="$3"
    local expected_output="$4"

    echo -n "Test [$name]: "
    
    set +e
    output=$($GWBASIC "$file" 2>&1)
    code=$?
    set -e

    if [ "$code" -ne "$expected_code" ]; then
        echo "FAILED (Expected exit code $expected_code, got $code)"
        FAILED=$((FAILED + 1))
        return
    fi

    if [ -n "$expected_output" ]; then
        if ! echo "$output" | grep -q "$expected_output"; then
            echo "FAILED (Output did not contain '$expected_output')"
            FAILED=$((FAILED + 1))
            return
        fi
    fi

    echo "PASSED (Exit code $code)"
}

run_test "PRINT Commands" "tests/test_print.bas" 0 "=== Testing PRINT ==="
run_test "Variables & Math" "tests/test_variables.bas" 0 "Result: 150"
run_test "Screen (CLS/LOCATE/COLOR)" "tests/test_screen.bas" 0 "Colored text at row 5 col 10"
run_test "REM & END Statements" "tests/test_rem_end.bas" 0 "Line before END"
run_test "SYSTEM Status '1'" "tests/test_system_1.bas" 0 "1"
run_test "SYSTEM Status 'OK'" "tests/test_system_ok.bas" 0 "OK"
run_test "Comprehensive Suite" "tests/run_all.bas" 0 "All Basic commands executed successfully."
run_test "Error Line Return Code" "tests/test_error_30.bas" 30 ""
run_test "Help Flag (-h)" "-h" 0 "Usage:"
run_test "Help Flag (--help)" "--help" 0 "Usage:"
run_test "Help Flag (-?)" "-?" 0 "Usage:"

echo "----------------------------------------"
if [ "$FAILED" -eq 0 ]; then
    echo "ALL TESTS PASSED!"
    exit 0
else
    echo "$FAILED TEST(S) FAILED!"
    exit 1
fi
