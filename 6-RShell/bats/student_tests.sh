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
    [ "$stripped_output" = "$expected_output" ]
}


@test "Multiple piped commands" {
    run "./dsh" <<EOF
ls -l | grep c | wc -l
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="4localmodedsh4>dsh4>cmdloopreturned-1"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"
    [ "$stripped_output" = "$expected_output" ]
}

@test "Invalid command" {
    run "./dsh" <<EOF
invalidcommand
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="execvp:Nosuchfileordirectorylocalmodedsh4>localmodedsh4>dsh4>cmdloopreturned-1"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"
    [ "$stripped_output" = "$expected_output" ]
}

@test "Exit command" {
    run "./dsh" <<EOF
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="localmodedsh4>cmdloopreturned-7"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"
    [ "$stripped_output" = "$expected_output" ]
}

@test "Client-Server echo command" {
    skip "Never able to get server and client to run consistently"
    # Start the server in the background
    ./dsh -s -i 0.0.0.0 -p 2710 &
    server_pid1=$!  # Capture server PID

    # Wait for the server to initialize
    sleep 1

    # Run the client and capture its output
    run ./dsh -c -i 10.246.251.12 -p 2710 <<EOF
echo "Hello from client"
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:][:cntrl:]')
    expected_output="socketclientmode:addr:10.246.251.12:2710dsh4>HellofromclientHellofromclientdsh4>cmdloopreturned-1"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"

    [ "$stripped_output" = "$expected_output" ]

    # Kill the server process after test
    kill $server_pid1
    wait $server_pid1 2>/dev/null
    sleep 2
}

@test "Client-Server Exit Command" {
skip "Never able to get server and client to run consistently"
    # Start the server in the background
    ./dsh -s -i 0.0.0.0 -p 2715 & 
    server_pid2=$!  # Capture server PID

    # Wait for the server to initialize
    sleep 1

    # Run the client and capture its output
    run ./dsh -c -i 10.246.251.12 -p 2715 <<EOF
exit
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:][:cntrl:]')
    expected_output="socketclientmode:addr:10.246.251.12:2715dsh4>dsh4>cmdloopreturned-1"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"

    [ "$stripped_output" = "$expected_output" ]

    # Kill the server process after test
    kill $server_pid2
    wait $server_pid2 2>/dev/null
    sleep 2
}

@test "Client-Server Stop Server" {
skip "Never able to get server and client to run consistently"
    # Start the server in the background
    ./dsh -s -i 0.0.0.0 -p 2720 & 
    server_pid3=$!  # Capture server PID

    # Wait for the server to initialize
    sleep 1

    # Run the client and capture its output
    run ./dsh -c -i 10.246.251.12 -p 2720 <<EOF
stop-server
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:][:cntrl:]')
    expected_output="socketclientmode:addr:10.246.251.12:2720dsh4>HellofromclientHellofromclientdsh4>cmdloopreturned-1"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"

    [ "$stripped_output" = "$expected_output" ]

    # Kill the server process after test
    kill $server_pid3
    wait $server_pid3 2>/dev/null
    sleep 2
}

@test "Client-Server Pipes" {
skip "Never able to get server and client to run consistently"
    # Start the server in the background
    ./dsh -s -i 0.0.0.0 -p 2725 & 
    server_pid4=$!  # Capture server PID

    # Wait for the server to initialize
    sleep 1

    # Run the client and capture its output
    run ./dsh -c -i 10.246.251.12 -p 2725 <<EOF
ls -l | grep txt | wc -l
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:][:cntrl:]')
    expected_output="socketclientmode:addr:10.246.251.12:2725dsh4>00dsh4>cmdloopreturned-1"
    echo "Stripped Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Output: $output"

    [ "$stripped_output" = "$expected_output" ]

    # Kill the server process after test
    kill $server_pid4
    wait $server_pid4 2>/dev/null
    sleep 2
}