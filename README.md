
1)
make s5pv210_defconfig

2)
make source

3)
make toolchain

4)
make V=1


make help
make menuconfig


dcron
crontab -c /etc/cron/crontabs -e
*/2 * * * * date > /dev/console
crontab -c /etc/cron/crontabs -l
