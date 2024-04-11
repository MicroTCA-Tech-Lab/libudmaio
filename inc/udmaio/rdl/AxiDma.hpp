/* This file was automatically generated with HECTARE v0.3.1
 *
 * DO NOT EDIT
 *
 *   input_filename = /mnt/hd2/projects/damc-fmc2zup-example-project/project/damc_fmc2zup_project.gen/sources_1/bd/system_app/ip/system_app_axi_dma_1/system_app_axi_dma_1.xml
 */

#pragma once

#include <cstdint>

#ifndef _HECTARE_PACKED
#define _HECTARE_PACKED __attribute__((packed, aligned(4)))
#endif  // _HECTARE_PACKED

/**
 * Reg
 *
 * Register Block
 *
*/
namespace axi_dma {

// structs for hardware access

/**
 * MM2S DMA Control Register
 *
 */
struct mm2s_dmacr_t {
    /// Run / Stop control for controlling running and stopping of the DMA channel.
    /// 0 - Stop – DMA stops when current (if any) DMA operations are complete. For Scatter / Gather Mode pending commands/transfers are flushed or completed.
    /// AXI4-Stream outs are potentially terminated early. Descriptors in the update queue are allowed to finish updating to remote memory before engine halt.
    /// For Direct Register mode pending commands/transfers are flushed or completed. AXI4-Stream outs are potentially terminated.
    /// The halted bit in the DMA Status register asserts to 1 when the DMA engine is halted. This bit is cleared by AXI DMA hardware when an error occurs. The CPU can also choose to clear this bit to stop DMA operations.
    /// 1 - Run – Start DMA operations. The halted bit in the DMA Status register deasserts to 0 when the DMA engine begins operations.
    bool rs : 1;
    unsigned rsvd_1_1 : 1;

    /// Soft reset for resetting the AXI DMA core. Setting this bit to a 1 causes the AXI DMA to be reset. Reset is accomplished gracefully. Pending commands/transfers are flushed or completed.
    /// AXI4-Stream outs are potentially terminated early. Setting either MM2S_DMACR. Reset = 1 or S2MM_DMACR.Reset = 1 resets the entire AXI DMA engine. After completion of a soft reset, all registers and bits are in the Reset State.      0 - Normal operation.   1 - Reset in progress.
    bool reset : 1;

    /// Keyhole Read. Setting this bit to 1 causes AXI DMA to initiate MM2S reads (AXI4read) in non-incrementing address mode (Fixed Address Burst transfer on AXI4). This bit can be updated when AXI DMA is in idle. When using keyhole operation the Max Burst Length should not exceed 16. This bit should not be set when DRE is enabled.
    /// This bit is non functional when the multichannel feature is enabled or in Direct Register mode.
    bool keyhole : 1;

    /// When set to 1, the DMA operates in Cyclic Buffer Descriptor (BD) mode without any user intervention. In this mode, the Scatter Gather module ignores the Completed bit of the BD. With this bit set, you can use the same BDs in cyclic manner without worrying about any stale descriptor errors.
    /// This bit should be set/unset only when the DMA is idle or when not running. Updating this bit while the DMA is running can result in unexpected behavior.
    /// This bit is non functional when DMA operates in multichannel mode.
    bool cyclic_bd_enable : 1;
    unsigned rsvd_11_5 : 7;

    /// Interrupt on Complete (IOC) Interrupt Enable. When set to 1, allows DMASR.IOC_Irq to generate an interrupt out for descriptors with the IOC bit set.   0 - IOC Interrupt disabled      1 - IOC Interrupt enabled
    bool ioc_irq_en : 1;

    /// Interrupt on Delay Timer Interrupt Enable. When set to 1, allows DMASR.Dly_Irq to generate an interrupt out.      0 - Delay Interrupt disabled   1 - Delay Interrupt enabled Note: This field is ignored when AXI DMA is configured for Direct Register Mode.
    bool dly_irq_en : 1;

    /// Interrupt on Error Interrupt Enable.
    /// 0 - Error Interrupt disabled
    /// 1 - Error Interrupt enabled
    bool err_irq_en : 1;
    unsigned rsvd_15_15 : 1;

    /// Interrupt Threshold. This value is used for setting the interrupt threshold. When IOC interrupt events occur, an internal counter counts down from the Interrupt Threshold setting. When the count reaches zero, an interrupt out is generated by the DMA engine.   Note: The minimum setting for the threshold is 0x01. A write of 0x00 to this register has no effect.   Note: This field is ignored when AXI DMA is configured for Direct Register Mode.
    uint8_t irq_threshold;

    /// Interrupt Delay Time Out. This value is used for setting the interrupt timeout value. The interrupt timeout mechanism causes the DMA engine to generate an interrupt after the delay time period has expired. Timer begins counting at the end of a packet and resets with receipt of a new packet or a timeout event occurs.
    /// Note: Setting this value to zero disables the delay timer interrupt.
    /// Note: This field is ignored when AXI DMA is configured for Direct Register Mode.
    uint8_t irq_delay;
} _HECTARE_PACKED;
static_assert(sizeof(mm2s_dmacr_t) == 4, "Contents of register mm2s_dmacr must be 4 bytes");

/**
 * MM2S DMA Status Register
 *
 */
struct mm2s_dmasr_t {
    /// DMA Channel Halted. Indicates the run/stop state of the DMA channel.      0 - DMA channel running.      1 - DMA channel halted.  For Scatter / Gather Mode this bit gets set when DMACR.RS = 0 and DMA and SG operations have halted. For Direct Register mode (C_INCLUDE_SG = 0) this bit gets set when DMACR.RS = 0 and DMA operations have halted. There can be a lag of time between when DMACR.RS = 0 and when DMASR.Halted = 1 Note: When halted (RS= 0 and Halted = 1), writing to CURDESC_PTR or TAILDESC_PTR pointer registers has no effect on DMA operations when in Scatter Gather Mode. For Direct Register Mode, writing to the LENGTH register has no effect on DMA operations.
    bool halted : 1;

    /// DMA Channel Idle. Indicates the state of AXI DMA operations.
    /// For Scatter / Gather Mode when IDLE indicates the SG Engine has reached the tail pointer for the associated channel and all queued descriptors have been processed. Writing to the tail pointer register automatically restarts DMA operations.
    /// For Direct Register Mode when IDLE indicates the current transfer has completed.      0 - Not Idle. For Scatter / Gather Mode, SG has not reached tail descriptor pointer and/or DMA operations in progress. For Direct Register Mode, transfer is not complete.      1 - Idle. For Scatter / Gather Mode, SG has reached tail descriptor pointer and DMA operation paused. for Direct Register Mode, DMA transfer has completed and controller is paused.  Note: This bit is 0 when channel is halted (DMASR.Halted=1). This bit is also 0 prior to initial transfer when AXI DMA configured for Direct Register Mode.
    bool idle : 1;
    unsigned rsvd_2_2 : 1;

    /// 1 - Scatter Gather Enabled
    /// 0 - Scatter Gather not enabled
    bool sg_incld : 1;

    /// DMA Internal Error. Internal error occurs if the buffer length specified in the fetched descriptor is set to 0. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No DMA Internal Errors        1 - DMA Internal Error detected. DMA Engine halts
    bool dma_int_err : 1;

    /// DMA Slave Error. This error occurs if the slave read from the Memory Map interface issues a Slave Error. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No DMA Slave Errors.      1 - DMA Slave Error detected. DMA Engine halts
    bool dma_slv_err : 1;

    /// DMA Decode Error. This error occurs if the address request points to an invalid address. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No DMA Decode Errors.      1 - DMA Decode Error detected. DMA Engine halts.
    bool dma_dec_err : 1;
    unsigned rsvd_7_7 : 1;

    /// Scatter Gather Internal Error. This error occurs if a descriptor with the “Complete bit” already set is fetched. Refer to the Scatter Gather Descriptor section for more information.This indicates to the SG Engine that the descriptor is a stale descriptor. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No SG Internal Errors.      1 - SG Internal Error detected. DMA Engine halts.  Note: This bit is not used and is fixed at 0 when AXI DMA is configured for Direct Register Mode.
    bool sg_int_err : 1;

    /// Scatter Gather Slave Error. This error occurs if the slave read from on the Memory Map interface issues a Slave error. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No SG Slave Errors.      1 - SG Slave Error detected. DMA Engine halts.  Note: This bit is not used and is fixed at 0 when AXI DMA is configured for Direct Register Mode.
    bool sg_slv_err : 1;

    /// Scatter Gather Decode Error. This error occurs if CURDESC_PTR and/or NXTDESC_PTR points to an invalid address. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No SG Decode Errors.      1 - SG Decode Error detected. DMA Engine halts.  Note: This bit is not used and is fixed at 0 when AXI DMA is configured for Direct Register Mode.
    bool sg_dec_err : 1;
    unsigned rsvd_11_11 : 1;

    /// Interrupt on Complete. When set to 1 for Scatter/Gather Mode, indicates an interrupt event was generated on completion of a descriptor. This occurs for descriptors with the End of Frame (EOF) bit set. When set to 1 for Direct Register Mode, indicates an interrupt event was generated on completion of a transfer. If the corresponding bit is enabled in the MM2S_DMACR (IOC_IrqEn = 1) and if the interrupt threshold has been met, causes an interrupt out to be generated from the AXI DMA.      0 - No IOC Interrupt.      1 - IOC Interrupt detected.  Writing a 1 to this bit will clear it.
    bool ioc_irq : 1;

    /// Interrupt on Delay. When set to 1, indicates an interrupt event was generated on delay timer time out. If the corresponding bit is enabled in the MM2S_DMACR (Dly_IrqEn = 1), an interrupt out is generated from the AXI DMA.      0 - No Delay Interrupt.      1 - Delay Interrupt detected. Note: This bit is not used and is fixed at 0 when AXI DMA is configured for Direct Register Mode.
    bool dly_irq : 1;

    /// Interrupt on Error. When set to 1, indicates an interrupt event was generated on error. If the corresponding bit is enabled in the MM2S_DMACR (Err_IrqEn = 1), an interrupt out is generated from the AXI DMA.
    /// Writing a 1 to this bit will clear it.
    /// 0 - No error Interrupt.
    /// 1 - Error interrupt detected.
    bool err_irq : 1;
    unsigned rsvd_15_15 : 1;

    /// Interrupt Threshold Status. Indicates current interrupt threshold value.
    /// Note: Applicable only when Scatter Gather is enabled.
    uint8_t irq_threshold_sts;

    /// Interrupt Delay Time Status. Indicates current interrupt delay time value.
    /// Note: Applicable only when Scatter Gather is enabled.
    uint8_t irq_delay_sts;
} _HECTARE_PACKED;
static_assert(sizeof(mm2s_dmasr_t) == 4, "Contents of register mm2s_dmasr must be 4 bytes");

/**
 * MM2S DMA Current Descriptor Pointer Register
 *
 */
struct mm2s_curdesc_t {
    unsigned rsvd_5_0 : 6;

    /// Indicates the pointer of the current descriptor being worked on. This register must contain a pointer to a valid descriptor prior to writing the TAILDESC_PTR register. Otherwise, undefined results occur. When DMACR.RS is 1, CURDESC_PTR becomes Read Only (RO) and is used to fetch the first descriptor.
    /// When the DMA Engine is running (DMACR.RS=1), CURDESC_PTR registers are updated by AXI DMA to indicate the current descriptor being worked on.
    /// On error detection, CURDESC_PTR is updated to reflect the descriptor associated with the detected error.
    /// Note: The register can only be written to by the CPU when the DMA Engine is Halted (DMACR.RS=0 and DMASR.Halted =1). At all other times, this register is Read Only (RO). Descriptors must be 16 word aligned, that is, 0x00, 0x40, 0x80 and others. Any other alignment has undefined results.
    unsigned current_descriptor_pointer : 26;
} _HECTARE_PACKED;
static_assert(sizeof(mm2s_curdesc_t) == 4, "Contents of register mm2s_curdesc must be 4 bytes");

/**
 * MM2S DMA Current Descriptor Pointer Register
 *
 */
using mm2s_curdesc_msb_t = uint32_t;
static_assert(sizeof(mm2s_curdesc_msb_t) == 4,
              "Contents of register mm2s_curdesc_msb must be 4 bytes");

/**
 * MM2S DMA Tail Descriptor Pointer Register
 *
 */
struct mm2s_taildesc_t {
    unsigned rsvd_5_0 : 6;

    /// Indicates the pause pointer in a descriptor chain. The AXI DMA SG Engine pauses descriptor fetching after completing operations on the descriptor whose current descriptor pointer matches the tail descriptor pointer.
    /// When AXI DMA Channel is not halted (DMASR.Halted = 0), a write by the CPU to the TAILDESC_PTR register causes the AXI DMA SG Engine to start fetching descriptors or restart if it was idle (DMASR.Idle = 1). If it was not idle, writing TAILDESC_PTR has no effect except to reposition the pause point.
    /// If the AXI DMA Channel is halted (DMASR.Halted = 1 and DMACR.RS = 0), a write by the CPU to the TAILDESC_PTR register has no effect except to reposition the pause point.
    /// Note: The software must not move the tail pointer to a location that has not been updated. The software processes and reallocates all completed descriptors (Cmplted = 1), clears the completed bits and then moves the tail pointer. The software must move the pointer to the last descriptor it updated. Descriptors must be 16-word aligned, that is, 0x00, 0x40, 0x80, and so forth. Any other alignment has undefined results.
    unsigned tail_descriptor_pointer : 26;
} _HECTARE_PACKED;
static_assert(sizeof(mm2s_taildesc_t) == 4, "Contents of register mm2s_taildesc must be 4 bytes");

/**
 * MM2S DMA Tail Descriptor Pointer Register
 *
 */
using mm2s_taildesc_msb_t = uint32_t;
static_assert(sizeof(mm2s_taildesc_msb_t) == 4,
              "Contents of register mm2s_taildesc_msb must be 4 bytes");

/**
 * MM2S Source Address Register
 *
 */
using mm2s_sa_t = uint32_t;
static_assert(sizeof(mm2s_sa_t) == 4, "Contents of register mm2s_sa must be 4 bytes");

/**
 * MM2S Source Address Register
 *
 */
using mm2s_sa_msb_t = uint32_t;
static_assert(sizeof(mm2s_sa_msb_t) == 4, "Contents of register mm2s_sa_msb must be 4 bytes");

/**
 * MM2S DMA Transfer Length Register
 *
 */
struct mm2s_length_t {
    /// Indicates the number of bytes to transfer for the MM2S channel. Writing a non-zero value to this register starts the MM2S transfer.
    unsigned length : 26;
    unsigned rsvd_31_26 : 6;
} _HECTARE_PACKED;
static_assert(sizeof(mm2s_length_t) == 4, "Contents of register mm2s_length must be 4 bytes");

/**
 * Scatter/Gather User and Cache Control Register
 *
 */
struct sg_ctl_t {
    /// Scatter/Gather Cache Control. Values written in this register reflect on the m_axi_sg_arcache and m_axi_sg_awcache signals of the M_AXI_SG interface.
    unsigned sg_cache : 4;
    unsigned rsvd_7_4 : 4;

    /// Scatter/Gather User Control. Values written in this register reflect on the m_axi_sg_aruser and m_axi_sg_awuser signals of the M_AXI_SG interface.
    unsigned sg_user : 4;
    unsigned rsvd_31_12 : 20;
} _HECTARE_PACKED;
static_assert(sizeof(sg_ctl_t) == 4, "Contents of register sg_ctl must be 4 bytes");

/**
 * S2MM DMA Control Register
 *
 */
struct s2mm_dmacr_t {
    /// Run / Stop control for controlling running and stopping of the DMA channel.
    /// 0 - Stop – DMA stops when current (if any) DMA operations are complete. For Scatter / Gather Mode pending commands/transfers are flushed or completed.
    /// AXI4-Stream outs are potentially terminated early. Descriptors in the update queue are allowed to finish updating to remote memory before engine halt.
    /// For Direct Register mode pending commands/transfers are flushed or completed. AXI4-Stream outs are potentially terminated. Data integrity on S2MM AXI4 cannot be guaranteed.
    /// The halted bit in the DMA Status register asserts to 1 when the DMA engine is halted. This bit is cleared by AXI DMA hardware when an error occurs. The CPU can also choose to clear this bit to stop DMA operations.
    /// 1 - Run – Start DMA operations. The halted bit in the DMA Status register deasserts to 0 when the DMA engine begins operations.
    bool rs : 1;
    unsigned rsvd_1_1 : 1;

    /// Soft reset for resetting the AXI DMA core. Setting this bit to a 1 causes the AXI DMA to be reset. Reset is accomplished gracefully. Pending commands/transfers are flushed or completed.
    /// AXI4-Stream outs are terminated early, if necessary with associated TLAST. Setting either MM2S_DMACR.Reset = 1 or S2MM_DMACR.Reset = 1 resets the entire AXI DMA engine. After completion of a soft reset, all registers and bits are in the Reset State.      0 - Reset not in progress. Normal operation.      1 - Reset in progress
    bool reset : 1;

    /// Keyhole Write. Setting this bit to 1 causes AXI DMA to initiate S2MM writes (AXI4 Writes) in non-incrementing address mode (Fixed Address Burst transfer on AXI4). This bit can be modified when AXI DMA is in idle. When enabling Key hole operation the maximum burst length cannot be more than 16. This bit should not be set when DRE is enabled.
    /// This bit is non functional when DMA is used in multichannel mode.
    bool keyhole : 1;

    /// When set to 1, the DMA operates in Cyclic Buffer Descriptor (BD) mode without any user intervention. In this mode, the Scatter Gather module ignores the Completed bit of the BD. With this bit set, you can use the same BDs in cyclic manner without worrying about any stale descriptor errors.
    /// This bit is non functional when DMA operates in Multichannel mode. or in Direct Register Mode
    bool cyclic_bd_enable : 1;
    unsigned rsvd_11_5 : 7;

    /// Interrupt on Complete (IOC) Interrupt Enable. When set to 1, allows Interrupt On Complete events to generate an interrupt out for descriptors with the Complete bit set.      0 - IOC Interrupt disabled      1 - IOC Interrupt enabled
    bool ioc_irq_en : 1;

    /// Interrupt on Delay Timer Interrupt Enable. When set to 1, allows error events to generate an interrupt out.      0 - Delay Interrupt disabled      1 - Delay Interrupt enabled  Note: Applicable only when Scatter Gather is enabled.
    bool dly_irq_en : 1;

    /// Interrupt on Error Interrupt Enable. When set to 1, allows error events to generate an interrupt out.      0 - Error Interrupt disabled      1 - Error Interrupt enabled
    bool err_irq_en : 1;
    unsigned rsvd_15_15 : 1;

    /// Interrupt Threshold. This value is used for setting the interrupt threshold. When IOC interrupt events occur, an internal counter counts down from the Interrupt Threshold setting. When the count reaches zero, an interrupt out is generated by the DMA engine.
    /// Note: The minimum setting for the threshold is 0x01. A write of 0x00 to this register has no effect.
    /// Note: Applicable only when Scatter Gather is enabled.
    uint8_t irq_threshold;

    /// Interrupt Delay Time Out. This value is used for setting the interrupt timeout value. The interrupt timeout mechanism causes the DMA engine to generate an interrupt after the delay time period has expired. Timer begins counting at the end of a packet and resets with receipt of a new packet or a timeout event occurs.
    /// Note: Setting this value to zero disables the delay timer interrupt.
    /// Note: Applicable only when Scatter Gather is enabled.
    uint8_t irq_delay;
} _HECTARE_PACKED;
static_assert(sizeof(s2mm_dmacr_t) == 4, "Contents of register s2mm_dmacr must be 4 bytes");

/**
 * S2MM DMA Status Register
 *
 */
struct s2mm_dmasr_t {
    /// DMA Channel Halted. Indicates the run/stop state of the DMA channel.      0 - DMA channel running.      1 - DMA channel halted.  For Scatter/Gather Mode this bit gets set when DMACR.RS = 0 and DMA and SG operations have halted. For Direct Register Mode this bit gets set when DMACR.RS = 0 and DMA operations have halted. There can be a lag of time between when DMACR.RS = 0 and when DMASR.Halted = 1
    /// Note: When halted (RS= 0 and Halted = 1), writing to CURDESC_PTR or TAILDESC_PTR pointer registers has no effect on DMA operations when in Scatter Gather Mode. For Direct Register Mode, writing to the LENGTH register has no effect on DMA operations.
    bool halted : 1;

    /// DMA Channel Idle. Indicates the state of AXI DMA operations.
    /// For Scatter / Gather Mode when IDLE indicates the SG Engine has reached the tail pointer for the associated channel and all queued descriptors have been processed. Writing to the tail pointer register automatically restarts DMA operations.
    /// For Direct Register Mode when IDLE indicates the current transfer has completed.      0 - Not Idle.      1 - Idle.   Note: This bit is 0 when channel is halted (DMASR.Halted=1). This bit is also 0 prior to initial transfer when AXI DMA configured for Direct Register Mode.
    bool idle : 1;
    unsigned rsvd_2_2 : 1;

    /// Scatter Gather Engine Included. DMASR.SGIncld = 1 indicates the Scatter Gather engine is included and the AXI DMA is configured for Scatter Gather mode. DMASR.SGIncld = 0 indicates the Scatter Gather engine is excluded and the AXI DMA is configured for Direct Register Mode.
    bool sg_incld : 1;

    /// DMA Internal Error. This error occurs if the buffer length specified in the fetched descriptor is set to 0. Also, when in Scatter Gather Mode and using the status app length field, this error occurs when the Status AXI4-Stream packet RxLength field does not match the S2MM packet being received by the S_AXIS_S2MM interface. When Scatter Gather is disabled, this error is flagged if any error occurs during Memory write or if the incoming packet is bigger than what is specified in the DMA length register.
    /// This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No DMA Internal Errors      1 - DMA Internal Error detected.
    bool dma_int_err : 1;

    /// DMA Slave Error. This error occurs if the slave read from the Memory Map interface issues a Slave Error. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No DMA Slave Errors.      1 - DMA Slave Error detected.
    bool dma_slv_err : 1;

    /// DMA Decode Error. This error occurs if the address request points to an invalid address. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.     0 - No DMA Decode Errors.   1 - DMA Decode Error detected.
    bool dma_dec_err : 1;
    unsigned rsvd_7_7 : 1;

    /// Scatter Gather Internal Error. This error occurs if a descriptor with the “Complete bit” already set is fetched. This indicates to the SG Engine that the descriptor is a tail descriptor. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No SG Internal Errors.      1 - SG Internal Error detected.  Note: Applicable only when Scatter Gather is enabled.
    bool sg_int_err : 1;

    /// Scatter Gather Slave Error. This error occurs if the slave read from on the Memory Map interface issues a Slave error. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No SG Slave Errors.      1 - SG Slave Error detected. DMA Engine halts. Note: Applicable only when Scatter Gather is enabled.
    bool sg_slv_err : 1;

    /// Scatter Gather Decode Error. This error occurs if CURDESC_PTR and/or NXTDESC_PTR points to an invalid address. This error condition causes the AXI DMA to halt gracefully. The DMACR.RS bit is set to 0, and when the engine has completely shut down, the DMASR.Halted bit is set to 1.      0 - No SG Decode Errors.      1 - SG Decode Error detected. DMA Engine halts. Note: Applicable only when Scatter Gather is enabled.
    bool sg_dec_err : 1;
    unsigned rsvd_11_11 : 1;

    /// Interrupt on Complete. When set to 1 for Scatter/Gather Mode, indicates an interrupt event was generated on completion of a descriptor. This occurs for descriptors with the End of Frame (EOF) bit set. When set to 1 for Direct Register Mode, indicates an interrupt event was generated on completion of a transfer. If the corresponding bit in S2MM_DMACR is enabled (IOC_IrqEn = 1) and if the interrupt threshold has been met, causes an interrupt out to be generated from the AXI DMA.      0 - No IOC Interrupt.      1 - IOC Interrupt detected. Writing a 1 to this bit will clear it.
    bool ioc_irq : 1;

    /// Interrupt on Delay. When set to 1, indicates an interrupt event was generated on delay timer time out. If the corresponding bit is enabled in the S2MM_DMACR (Dly_IrqEn = 1), an interrupt out is generated from the AXI DMA.      0 - No Delay Interrupt.      1 - Delay Interrupt detected.1 = IOC Interrupt detected. Writing a 1 to this bit will clear it. Note: Applicable only when Scatter Gather is enabled.
    bool dly_irq : 1;

    /// Interrupt on Error. When set to 1, indicates an interrupt event was generated on error. If the corresponding bit is enabled in the S2MM_DMACR (Err_IrqEn = 1), an interrupt out is generated from the AXI DMA.
    /// Writing a 1 to this bit will clear it.      0 - No error Interrupt.      1 - Error interrupt detected.
    bool err_irq : 1;
    unsigned rsvd_15_15 : 1;

    /// Interrupt Threshold Status. Indicates current interrupt threshold value.
    /// Note: Applicable only when Scatter Gather is enabled.
    uint8_t irq_threshold_sts;

    /// Interrupt Delay Time Status. Indicates current interrupt delay time value.
    /// Note: Applicable only when Scatter Gather is enabled.
    uint8_t irq_delay_sts;
} _HECTARE_PACKED;
static_assert(sizeof(s2mm_dmasr_t) == 4, "Contents of register s2mm_dmasr must be 4 bytes");

/**
 * S2MM DMA Current Descriptor Pointer Register
 *
 */
struct s2mm_curdesc_t {
    unsigned rsvd_5_0 : 6;

    /// Indicates the pointer of the current descriptor being worked on. This register must contain a pointer to a valid descriptor prior to writing the TAILDESC_PTR register. Otherwise, undefined results occur. When DMACR.RS is 1, CURDESC_PTR becomes Read Only (RO) and is used to fetch the first descriptor.
    /// When the DMA Engine is running (DMACR.RS=1), CURDESC_PTR registers are updated by AXI DMA to indicate the current descriptor being worked on.
    /// On error detection, CURDESC_PTR is updated to reflect the descriptor associated with the detected error.
    /// Note: The register can only be written to by the CPU when the DMA Engine is Halted (DMACR.RS=0 and DMASR.Halted =1). At all other times, this register is Read Only (RO).
    /// Buffer Descriptors must be 16 word aligned, that is, 0x00, 0x40, 0x80 and so forth. Any other alignment has undefined results.
    unsigned current_descriptor_pointer : 26;
} _HECTARE_PACKED;
static_assert(sizeof(s2mm_curdesc_t) == 4, "Contents of register s2mm_curdesc must be 4 bytes");

/**
 * S2MM DMA Current Descriptor Pointer Register
 *
 */
using s2mm_curdesc_msb_t = uint32_t;
static_assert(sizeof(s2mm_curdesc_msb_t) == 4,
              "Contents of register s2mm_curdesc_msb must be 4 bytes");

/**
 * S2MM DMA Tail Descriptor Pointer Register
 *
 */
struct s2mm_taildesc_t {
    unsigned rsvd_5_0 : 6;

    /// Indicates the pause pointer in a descriptor chain. The AXI DMA SG Engine pauses descriptor fetching after completing operations on the descriptor whose current descriptor pointer matches the tail descriptor pointer.
    /// When AXI DMA Channel is not halted (DMASR.Halted = 0), a write by the CPU to the TAILDESC_PTR register causes the AXI DMA SG Engine to start fetching descriptors or restart if it was idle (DMASR.Idle = 1). If it was not idle, writing TAILDESC_PTR has no effect except to reposition the pause point.
    /// If the AXI DMA Channel DMACR.RS bit is set to 0 (DMASR.Halted = 1 and DMACR.RS = 0), a write by the CPU to the TAILDESC_PTR register has no effect except to reposition the pause point.
    /// Note: The software must not move the tail pointer to a location that has not been updated. The software processes and reallocates all completed descriptors (Cmplted = 1), clears the completed bits and then moves the tail pointer. The software must move the pointer to the last descriptor it updated.
    /// Descriptors must be 16-word aligned, that is, 0x00, 0x40, 0x80, and so forth. Any other alignment has undefined results.
    unsigned tail_descriptor_pointer : 26;
} _HECTARE_PACKED;
static_assert(sizeof(s2mm_taildesc_t) == 4, "Contents of register s2mm_taildesc must be 4 bytes");

/**
 * S2MM DMA Tail Descriptor Pointer Register
 *
 */
using s2mm_taildesc_msb_t = uint32_t;
static_assert(sizeof(s2mm_taildesc_msb_t) == 4,
              "Contents of register s2mm_taildesc_msb must be 4 bytes");

/**
 * S2MM DMA Destination Address Register
 *
 */
using s2mm_da_t = uint32_t;
static_assert(sizeof(s2mm_da_t) == 4, "Contents of register s2mm_da must be 4 bytes");

/**
 * S2MM Destination Address Register
 *
 */
using s2mm_da_msb_t = uint32_t;
static_assert(sizeof(s2mm_da_msb_t) == 4, "Contents of register s2mm_da_msb must be 4 bytes");

/**
 * S2MM DMA Transfer Length Register
 *
 */
struct s2mm_length_t {
    /// Indicates the length in bytes of the S2MM buffer available to write receive data from the S2MM channel. Writing a non-zero value to this register enables S2MM channel to receive packet data.
    /// At the completion of the S2MM transfer, the number of actual bytes written on the S2MM AXI4 interface is updated to the S2MM_LENGTH register.
    /// Note: This value must be greater than or equal to the largest expected packet to be received on S2MM AXI4-Stream. Values smaller than the received packet result in undefined behavior.
    unsigned length : 26;
    unsigned rsvd_31_26 : 6;
} _HECTARE_PACKED;
static_assert(sizeof(s2mm_length_t) == 4, "Contents of register s2mm_length must be 4 bytes");

// Convenience template and wrapper

/**
 * @brief Template class for the axi_dma block, containing accessors for all its registers
 *
 * @tparam R_array_acc Register accessor template (provided by user), receiving the register type as parameter
 * @tparam R_acc_arg Constructor argument type for the register accessor (e.g. hardware-dependent information)
 *
 */
template <template <typename, uint32_t, uint32_t, uint32_t> typename R_array_acc,
          typename R_acc_arg>
struct block_template {
    template <typename R, uint32_t O>
    using R_acc = R_array_acc<R, O, 0, 0>;

    /// Register accessor for MM2S DMA Control Register
    R_acc<mm2s_dmacr_t, 0x0> mm2s_dmacr;
    /// Register accessor for MM2S DMA Status Register
    R_acc<mm2s_dmasr_t, 0x4> mm2s_dmasr;
    /// Register accessor for MM2S DMA Current Descriptor Pointer Register
    R_acc<mm2s_curdesc_t, 0x8> mm2s_curdesc;
    /// Register accessor for MM2S DMA Current Descriptor Pointer Register
    R_acc<mm2s_curdesc_msb_t, 0xc> mm2s_curdesc_msb;
    /// Register accessor for MM2S DMA Tail Descriptor Pointer Register
    R_acc<mm2s_taildesc_t, 0x10> mm2s_taildesc;
    /// Register accessor for MM2S DMA Tail Descriptor Pointer Register
    R_acc<mm2s_taildesc_msb_t, 0x14> mm2s_taildesc_msb;
    /// Register accessor for MM2S Source Address Register
    R_acc<mm2s_sa_t, 0x18> mm2s_sa;
    /// Register accessor for MM2S Source Address Register
    R_acc<mm2s_sa_msb_t, 0x1c> mm2s_sa_msb;
    /// Register accessor for MM2S DMA Transfer Length Register
    R_acc<mm2s_length_t, 0x28> mm2s_length;
    /// Register accessor for Scatter/Gather User and Cache Control Register
    R_acc<sg_ctl_t, 0x2c> sg_ctl;
    /// Register accessor for S2MM DMA Control Register
    R_acc<s2mm_dmacr_t, 0x30> s2mm_dmacr;
    /// Register accessor for S2MM DMA Status Register
    R_acc<s2mm_dmasr_t, 0x34> s2mm_dmasr;
    /// Register accessor for S2MM DMA Current Descriptor Pointer Register
    R_acc<s2mm_curdesc_t, 0x38> s2mm_curdesc;
    /// Register accessor for S2MM DMA Current Descriptor Pointer Register
    R_acc<s2mm_curdesc_msb_t, 0x3c> s2mm_curdesc_msb;
    /// Register accessor for S2MM DMA Tail Descriptor Pointer Register
    R_acc<s2mm_taildesc_t, 0x40> s2mm_taildesc;
    /// Register accessor for S2MM DMA Tail Descriptor Pointer Register
    R_acc<s2mm_taildesc_msb_t, 0x44> s2mm_taildesc_msb;
    /// Register accessor for S2MM DMA Destination Address Register
    R_acc<s2mm_da_t, 0x48> s2mm_da;
    /// Register accessor for S2MM Destination Address Register
    R_acc<s2mm_da_msb_t, 0x4c> s2mm_da_msb;
    /// Register accessor for S2MM DMA Transfer Length Register
    R_acc<s2mm_length_t, 0x58> s2mm_length;

    block_template(R_acc_arg arg)
        : mm2s_dmacr{arg}
        , mm2s_dmasr{arg}
        , mm2s_curdesc{arg}
        , mm2s_curdesc_msb{arg}
        , mm2s_taildesc{arg}
        , mm2s_taildesc_msb{arg}
        , mm2s_sa{arg}
        , mm2s_sa_msb{arg}
        , mm2s_length{arg}
        , sg_ctl{arg}
        , s2mm_dmacr{arg}
        , s2mm_dmasr{arg}
        , s2mm_curdesc{arg}
        , s2mm_curdesc_msb{arg}
        , s2mm_taildesc{arg}
        , s2mm_taildesc_msb{arg}
        , s2mm_da{arg}
        , s2mm_da_msb{arg}
        , s2mm_length{arg} {}
};

/**
 * @brief Convenience wrapper for block_template to omit the explicit naming of the constructor argument
 *
 * @tparam R_array_acc Register accessor template (provided by user), receiving the register type as parameter
 * R_acc must define a constructor_arg_type,
 * e.g. `using constructor_arg_type = std::reference_wrapper<HwAccessor>;`
 */
template <template <typename, uint32_t, uint32_t, uint32_t> typename R_array_acc>
using block =
    block_template<R_array_acc, typename R_array_acc<mm2s_dmacr_t, 0, 0, 0>::constructor_arg_type>;

}  // namespace axi_dma