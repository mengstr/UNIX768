#ifndef V7_SYS_STAT_H
#define V7_SYS_STAT_H

struct	stat
{
	dev_t	st_dev;
	ino_t	st_ino;
	mode_t	st_mode;
	short	st_nlink;
	short  	st_uid;
	short  	st_gid;
	dev_t	st_rdev;
	off_t	st_size;
	time_t	st_atime;
	time_t	st_mtime;
	time_t	st_ctime;
};

#define	S_IFMT	0170000		/* type of file */
#define		S_IFDIR	0040000	/* directory */
#define		S_IFCHR	0020000	/* character special */
#define		S_IFBLK	0060000	/* block special */
#define		S_IFREG	0100000	/* regular */
#define		S_IFMPC	0030000	/* multiplexed char special */
#define		S_IFMPB	0070000	/* multiplexed block special */
#define	S_ISUID	0004000		/* set user id on execution */
#define	S_ISGID	0002000		/* set group id on execution */
#define	S_ISVTX	0001000		/* save swapped text even after use */
#define	S_IREAD	0000400		/* read permission, owner */
#define	S_IWRITE	0000200		/* write permission, owner */
#define	S_IEXEC	0000100		/* execute/search permission, owner */

#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC
#define S_IRGRP (S_IREAD >> 3)
#define S_IWGRP (S_IWRITE >> 3)
#define S_IXGRP (S_IEXEC >> 3)
#define S_IROTH (S_IREAD >> 6)
#define S_IWOTH (S_IWRITE >> 6)
#define S_IXOTH (S_IEXEC >> 6)
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)

#endif /* V7_SYS_STAT_H */
