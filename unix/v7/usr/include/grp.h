struct	group { /* see getgrent(3) */
	char	*gr_name;
	char	*gr_passwd;
	i32	gr_gid;
	char	**gr_mem;
};
