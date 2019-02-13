#include "woody.h"

char *get_file(char *name, off_t *file_size)
{
	struct stat metadata;
	int fd;
	char *file;

	if ((fd = open(name, O_RDONLY)) == -1)
	{
		dprintf(2, "error during the open\n");
		return (NULL);
	}
	if (fstat(fd, &metadata) == -1)
	{
		dprintf(2, "ftsat failed\n");
		return (NULL);
	}
	*file_size = metadata.st_size;
	if ((file = mmap(0, (size_t)*file_size, PROT_READ,
					 MAP_PRIVATE, fd, 0)) == MAP_FAILED)
	{
		dprintf(2, "mmap failed\n");
		return (NULL);
	}
	close(fd);
	return (file);
}

int main(int ac, char **av)
{
	char *file;
	char *new_file;
	off_t file_size;
	void *ptr = NULL;
	Elf32_Shdr *shdr;
	Elf64_Shdr *text;

	if (ac < 2)
	{
		dprintf(2, "Usage: %s [FILENAME]\n", av[0]);
		return (1);
	}
	file_size = 0;
	file = get_file(av[1], &file_size);
	if (!file)
		return (1);
	printf("Sucess retreiving %s, file size: %zu bytes.\n", av[1], file_size);

	// Step 1 : generate our code with the good address
	// char *code = get_our_code(file, file_size)

	// Step 2 : find where to place our code
	// off_t entry = find_place(char *bin, off_t code_size);
	off_t cave_size;
	off_t entry;
	entry = find_cave(file, file_size, 0, &cave_size);

	printf("bigest cave entry: %jd, cave size: %jd\n", entry, cave_size);

	// Step 3 : copy our code (if we found a place, else we )
	// memcpy(bin + entry, our_code, code_length)

	new_file = inject_code(file, file_size, entry, cave_size);
	text = find_sect(file, ".text");
	if (!text)
	{
		dprintf(2, "No text section\n");
		return (1);
	}
	print_section(new_file, text);
	encrypt_section(new_file, text);
	printf("encrypted section: (key: \"%s\")\n", KEY);
	print_section(new_file, text);

	printf("exiting...\n");

	return 0;
}
