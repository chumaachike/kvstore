import socket
import subprocess
import time
import pytest

SERVER_BIN = "./build/kvstore"
PORT = 8080


@pytest.fixture
def server():
    proc = subprocess.Popen(
        [SERVER_BIN, str(PORT)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )

    time.sleep(0.2)  # small delay so server can start

    yield proc

    proc.terminate()
    proc.wait(timeout=2)


def send_command(command):
    with socket.create_connection(("127.0.0.1", PORT), timeout=2) as sock:
        sock.sendall((command + "\n").encode())
        return sock.recv(4096).decode()


def test_set_get(server):
    response = send_command("SET name chuma")
    assert "OK" in response

    response = send_command("GET name")
    assert "chuma" in response


def test_unknown_command(server):
    response = send_command("BADCOMMAND")
    assert "ERR" in response


def test_quit(server):
    response = send_command("QUIT")
    assert "Bye" in response