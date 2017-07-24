setenv OS Darwin
setenv DIMDIR `pwd`
echo DIMDIR is set to $DIMDIR
setenv ODIR darwin
echo ODIR is set to $ODIR
alias TestServer $DIMDIR/$ODIR/testServer
alias TestClient $DIMDIR/$ODIR/testClient
alias Test_server $DIMDIR/$ODIR/test_server
alias Test_client $DIMDIR/$ODIR/test_client
alias Dns    $DIMDIR/$ODIR/dns
alias Dim_get_service $DIMDIR/$ODIR/dim_get_service
alias Dim_send_command $DIMDIR/$ODIR/dim_send_command
alias DimBridge    $DIMDIR/$ODIR/DimBridge
alias Did    $DIMDIR/$ODIR/did
mkdir darwin
patch src/tcpip.c < tcpip.c.patch
patch < makefile_common.patch
patch < makefile_dim.patch
make all