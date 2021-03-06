#include "navigator.h"
#include "../../MushLib/heap.h"
#include "../../MushCreator/file_system/structures.h"
#include "../../MushCreator/file_system/file_input.h"
#include "../kernel/pages.h"
#include "../../MushLib/stdio.h"
#include "../../MushLib/memory.h"


extern u_dword memory_start;
extern u_dword orbit_start;
static u_dword entry_point;

void print_seg(elf_program_header* header) {
    info("Segment info:\n")
    info("\tSegment type: %h\n", header->p_type)
    info("\tSegment offset: %h\n\n", header->p_offset)
    info("\tSegment virtual address: %h\n", header->p_vaddr)
    info("\tSegment physical address: %h\n\n", header->p_paddr)
    info("\tSegment size in file: %h\n", header->p_filesz)
    info("\tSegment size in memory: %h\n\n", header->p_memsz)
    info("\tSegment flags: %h\n", header->p_flags)
    info("\tSegment alignment: %h\n\n", header->alignment)
}

void domestic_launch(string filename, u_dword slot) {
    file* executable = open_file_global(filename);
    info("Launching %s\n", get_name(executable))
    elf_file_header* header = (elf_file_header*) malloc(sizeof(elf_file_header));
    read_bytes(executable, (byte*) header, sizeof(elf_file_header), 0);
    info("Executable contains %d segments (at %h)\n", header->program_header_number, header->program_header_offset)

    elf_program_header* program_headers = (elf_program_header*) malloc(sizeof(elf_program_header) * header->program_header_number);
    seek_to(executable, header->program_header_offset);
    read_bytes(executable, (byte*) program_headers, size(program_headers), 0);

    u_dword slot_number = orbit_start + (slot * page_size);
    for (int j = slot_number; j < slot_number + (page_size * 8); j += page_size) get_page_address(j);
    memory_clear((byte*) slot_number, page_size * 8, 0);

    for (int i = 0; i < header->program_header_number; ++i) {
        if (program_headers[i].p_type == 1) {
            // print_seg(&(program_headers[i]));
            u_dword slot_address = program_headers[i].p_vaddr;
            u_dword data_size = program_headers[i].p_filesz;
            u_dword data_offset = program_headers[i].p_offset;
            if ((slot_number >= orbit_start) && (data_size < (page_size * 4)) && (slot_address + data_size < memory_start)) {
                info("Transferring %h bytes from offset %h in file to pos %h on memory...\n", data_size, data_offset, slot_address)
                seek_to(executable, data_offset);
                read_bytes(executable, (void*) slot_address, data_size, 0);
            } else {
                bad("Fatal error: segment in orbital app %s is wrongly positioned.\n", get_name(executable))
                bad("%h <= %h\n", program_headers[i].p_vaddr, orbit_start)
                bad("%h >= %h\n", program_headers[i].p_vaddr + program_headers[i].p_filesz, memory_start)
                return;
            }
        } else info("Skipping non-loadable segment (type %h)...\n", program_headers[i].p_type)
    }

    entry_point = header->entry;
    info("Calling slot %d, point %p\n", slot, entry_point)
    move_stack_to(slot_number + (page_size * 4))
    asm volatile ("call *%0" :: "r"(entry_point));
    move_stack_back()
}
