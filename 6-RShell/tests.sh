#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run "./dsh" <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Empty input" {
    run "./dsh" <<EOF

EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="localmodedsh4>warning:nocommandsprovidedcmdloopreturned-1"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"
    echo "Exit Status: $status"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Exceeding max arguments" {
    run "./dsh" <<EOF
one two three four five six seven eight nine ten
EOF
    echo "Exit Status: $status"
    [ "$status" -eq 0 ]
}

@test "Handling quoted arguments" {
    run "./dsh" <<EOF
echo "Hello World"
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="HelloWorldlocalmodedsh4>dsh4>cmdloopreturned-1"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"
    echo "Exit Status: $status"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}


@test "Multiple piped commands" {
    run "./dsh" <<EOF
ls | grep dshlib.c
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dshlib.c"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"
    echo "Exit Status: $status"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Invalid command" {
    run "./dsh" <<EOF
invalidcommand
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="execvp:Nosuchfileordirectorydsh3>dsh3>dsh3>cmdloopreturned-1"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"
    echo "Exit Status: $status"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}