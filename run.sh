echo 'Started run.sh'
echo 'Setting up all NFS requirements'
mkdir /run/sendsigs.omit.d/ 
/etc/init.d/rpcbind start 
start-stop-daemon --stop --oknodo --quiet --name rpc.idmapd 
start-stop-daemon --start --oknodo --quiet --exec /usr/sbin/rpc.idmapd 
/etc/init.d/nfs-common start 
echo 'All NFS requirements setup'

echo 'Started NFS mount'
mkdir /workspaces/remote
mount -t nfs -o vers=4 10.0.0.98:/volume1/codes /workspaces/remote 
echo 'NFS mounted'

echo 'Running script'
echo '=============='
#ln -s /workspaces/mlab/remote/impv-hpc /workspaces/mlab/out 
ln -s /workspaces/remote/memetico/ /workspaces/memetico
#python /workspaces/execute.py
echo 'Ending script and existing'