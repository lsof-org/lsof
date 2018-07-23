#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <sys/sysmacros.h>

extern int errno;

main(argc, argv)
	int argc;
	char *argv[];
{
	struct group *gr;
	int i;
	struct passwd *pw;
	struct stat sbuf;

	for (i = 1; i < argc; i++) {
		printf("%s:\n", argv[i]);
		if (stat(argv[i], &sbuf) != 0)
		{
			printf("  can't stat %s\n", strerror(errno));
			continue;
		}
		printf("  st_dev = (%d,%d) (%#x,%#x) (%#x)\n",
			major(sbuf.st_dev), minor(sbuf.st_dev),
			major(sbuf.st_dev), minor(sbuf.st_dev),
			sbuf.st_dev);
		printf("  st_ino = %ld (%#lx)\n", sbuf.st_ino, sbuf.st_ino);
		printf("  st_mode = 0%lo\n", sbuf.st_mode);
		printf("  st_uid = %ld", sbuf.st_uid);
		if ((pw = getpwuid(sbuf.st_uid)) != NULL)
			printf(" (%s)", pw->pw_name);
		putchar('\n');
		printf("  st_gid = %ld", sbuf.st_gid);
		if ((gr = getgrgid(sbuf.st_gid)) != NULL)
			printf(" (%s)", gr->gr_name);
		putchar('\n');
		printf("  st_rdev = (%d,%d) (%#x,%#x) (%#x)\n",
			major(sbuf.st_rdev), minor(sbuf.st_rdev),
			major(sbuf.st_rdev), minor(sbuf.st_rdev),
			sbuf.st_rdev);
		printf("  st_size = %ld (%#lx)\n", (long)sbuf.st_size,
			(long)sbuf.st_size);
		printf("  st_atime = %s", ctime(&sbuf.st_atime));
		printf("  st_mtime = %s", ctime(&sbuf.st_mtime));
		printf("  st_ctime = %s", ctime(&sbuf.st_ctime));
		printf("  st_nlink = %d\n", sbuf.st_nlink);
	}
	return(0);
}
