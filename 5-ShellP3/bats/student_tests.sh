#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Empty input" {
    run "./dsh3" <<EOF

EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="warning:nocommandsprovided"
    echo "Output: $output"
    echo "Exit Status: $status"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Exceeding max arguments" {
    CMD="echo $(printf 'arg%.0s ' {1..300})"
    run "./dsh3" <<EOF
$CMD
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="error:commandorargumentstoobig"
    echo "Output: $output"
    echo "Exit Status: $status"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Handling quoted arguments" {
    run "./dsh3" <<EOF
echo "Hello World"
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="HelloWorld"
    echo "Output: $output"
    echo "Exit Status: $status"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Multiple piped commands" {
    run "./dsh3" <<EOF
echo hello | grep h | wc -c
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="6"
    echo "Output: $output"
    echo "Exit Status: $status"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Invalid command" {
    run "./dsh3" <<EOF
invalidcommand
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="execvp:Nosuchfileordirectory"
    echo "Output: $output"
    echo "Exit Status: $status"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}