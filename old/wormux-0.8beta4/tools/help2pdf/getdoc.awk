BEGIN {
    toshow = 1;
}

// {
    if (toshow==1)
	print $0;
}

/<meta http-equiv/ {
    toshow = 0
}

/<div id\=\"content\"/ {
    toshow = 1
    print "<body>"
    print $0

}

/end of CONTENT div/ {
    toshow = 0
    print "</body>\n</html>"
}

