sudo apt install $(apt-cache pkgnames | grep -E -e "libcrypto\+\+.*([0-9]+|dev|dbg)\b")
