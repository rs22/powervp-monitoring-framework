#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define KEXCMD_HCALL		1		/* Perform phyp hcall */
#define KEXCMD_H24X7CATALOG	2		/* Perform phyp hcall */
#define KEXCMD_H24X7DATA	3		/* Perform phyp hcall */

#define H_SUCCESS	0
#define H_BUSY		1	/* Hardware busy -- retry later */
#define H_CLOSED	2	/* Resource closed */
#define H_NOT_AVAILABLE 3
#define H_CONSTRAINED	4	/* Resource request constrained to max allowed */
#define H_PARTIAL       5
#define H_IN_PROGRESS	14	/* Kind of like busy */
#define H_PAGE_REGISTERED 15
#define H_PARTIAL_STORE   16
#define H_PENDING	17	/* returned from H_POLL_PENDING */
#define H_CONTINUE	18	/* Returned from H_Join on success */
#define H_LONG_BUSY_START_RANGE		9900  /* Start of long busy range */
#define H_LONG_BUSY_ORDER_1_MSEC	9900  /* Long busy, hint that 1msec \
						 is a good time to retry */
#define H_LONG_BUSY_ORDER_10_MSEC	9901  /* Long busy, hint that 10msec \
						 is a good time to retry */
#define H_LONG_BUSY_ORDER_100_MSEC 	9902  /* Long busy, hint that 100msec \
						 is a good time to retry */
#define H_LONG_BUSY_ORDER_1_SEC		9903  /* Long busy, hint that 1sec \
						 is a good time to retry */
#define H_LONG_BUSY_ORDER_10_SEC	9904  /* Long busy, hint that 10sec \
						 is a good time to retry */
#define H_LONG_BUSY_ORDER_100_SEC	9905  /* Long busy, hint that 100sec \
						 is a good time to retry */
#define H_LONG_BUSY_END_RANGE		9905  /* End of long busy range */


typedef struct CounterValue {
    uint32_t  hardware_chip_id;
    uint32_t  reserved [3];
    uint64_t  idle_cycles_for_A_link;
    uint64_t  time_value_data_for_A_link_was_collected;
    uint64_t  idle_cycles_for_B_link;
    uint64_t  time_value_data_for_B_link_was_collected;
    uint64_t  idle_cycles_for_C_link;
    uint64_t  time_value_data_for_C_link_was_collected;
} counter_value_t;

typedef struct 
{
    uint64_t partition_id;
    uint64_t entitled_PURR_cycles_since_boot;
    uint64_t capped_PURR_cycles_consumed_since_boot;
    uint64_t uncapped_PURR_cycles_consumed_since_boot;
    uint64_t donated_PURR_cycles_since_boot;
    uint64_t idle_PURR_cycles;
} entitled_capped_uncapped_donated_idle_PURR_by_partition_ctr_value_t;

typedef struct
{
    uint64_t partition_id;
    uint64_t entitled_PURR_cycles_since_boot;
    uint64_t capped_PURR_cycles_consumed_since_boot;
} run_instructions_run_cycles_by_partition_ctr_value_t;

typedef struct Performance_Counter_Info_Parms {
    uint32_t requested_information;
    int32_t starting_index;
    uint32_t returned_values;
    uint32_t reserved1;
    uint64_t reserved2[2];
    run_instructions_run_cycles_by_partition_ctr_value_t counter_value;
} performance_counter_info_parms_t;

typedef struct KEXCMD {
    uint64_t cmd;                   /* See defines above */
    uint64_t rc;                    /* Actual rc from hcall */
    uint64_t token;                 /* hcall "token" */
    uint64_t size;                  /* Size of buffer */
    uint64_t buffer;
    uint64_t sizer;                 /* size of request buffer */
    uint64_t bufferr;               /* request buffer */
    uint64_t page;
    uint64_t version;
} kexcmd_t;

int main(int argc, char ** argv)
{
    int powervp = open("/dev/powervp", O_RDWR);

    if (powervp == 0) {
        printf("Could not open /dev/powervp\n");
        return 1;
    }

    int partition = -1;
    performance_counter_info_parms_t outbuf = { 0x30, partition };

    kexcmd_t cmd = {
        KEXCMD_HCALL,
        0,
        0xF080,
        sizeof(outbuf),
        (uint64_t)&outbuf,
        0, 0, 0, 0
    };

    write(powervp, &cmd, sizeof(cmd));
    if (cmd.rc != H_SUCCESS)
    {
        printf("Hypercall was not successful - return code %lu\n", cmd.rc);
    	close(powervp);
        return 1;
    }

    printf("Returned values %d\n", outbuf.returned_values);
    close(powervp);
    return 0;
}