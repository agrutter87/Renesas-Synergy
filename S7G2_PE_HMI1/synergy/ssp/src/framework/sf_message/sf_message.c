/***********************************************************************************************************************
 * Copyright [2015-2021] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 * 
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : sf_message.c
 * Description  : SSP messaging framework
 **********************************************************************************************************************/


/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "sf_message.h"
#include "sf_message_private_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** Macro for error logger. */
#ifndef SF_MESSAGE_ERROR_RETURN
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define SF_MESSAGE_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), &g_module_name[0], &module_version)
#endif

/** Bytes for controlling a queue in the message queue */
#define SF_MESSAGE_SIZE_FOR_UCHAR_POINTER_IN_BLOCK_POOL (4U)

/** This is the interrupt mask level to let the framework enter the critical section.
 *  @note Applications have to set the IRQ level grater than this level to get the framework work properly.
 *        The lower 4bits are fixed to zero in the Synergy device configuration */
#define IRQ_MASK_LEVEL_FOR_CRITICAL_SECTION             (1U << 4)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
#if (SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE)
static ssp_err_t sf_message_open_param_check (sf_message_instance_ctrl_t * const p_ctrl, sf_message_cfg_t const * const p_cfg);
#endif

static ssp_err_t sf_message_memory_pool_create (sf_message_instance_ctrl_t * const p_ctrl,
                                                sf_message_cfg_t const * const     p_cfg);

static ssp_err_t sf_message_queue_send (TX_QUEUE const * const              p_queue,
                                        sf_message_header_t const * const   p_buffer,
                                        sf_message_priority_t const         priority,
                                        uint32_t const                      wait_option);

static uint16_t  sf_message_get_number_of_subscribers (sf_message_instance_ctrl_t * const p_ctrl,
                                                       sf_message_header_t const * const  p_buffer,
                                                       sf_message_subscriber_list_t **    pp_subscriber_list);
#if SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE
static ssp_err_t sf_message_parameter_check(sf_message_instance_ctrl_t * p_ctrl, sf_message_header_t const * const   p_buffer);
#endif
static ssp_err_t sf_message_send(sf_message_instance_ctrl_t * p_ctrl,
                                 sf_message_header_t const * const   p_buffer,
                                 sf_message_post_err_t  * const      p_post_err,
                                 uint32_t const                      wait_option,
                                 sf_message_subscriber_list_t ** pp_subscriber_list,
                                 sf_message_priority_t           priority,
                                 sf_message_buffer_ctrl_t     *  pbuffer_ctrl);
static void sf_message_release_options(sf_message_buffer_ctrl_t * pbuffer_ctrl,
                                    bool  *do_callback,bool *do_buffer_release,
                                    uint32_t *do_nack_reply,
                                    sf_message_release_option_t const option,
                                    void (** p_callback_cp)(sf_message_callback_args_t * p_arg));
/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/** Name of module used by error logger macro */
#if BSP_CFG_ERROR_LOG != 0
static const char g_module_name[] = "sf_message";
#endif

#if defined(__GNUC__)
/* This structure is affected by warnings from a GCC compiler bug.  This pragma suppresses the warnings in this
 * structure only.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
/** Version data structure */
static const ssp_version_t module_version =
{
    .api_version_minor  = SF_MESSAGE_API_VERSION_MINOR,
    .api_version_major  = SF_MESSAGE_API_VERSION_MAJOR,
    .code_version_major = SF_MESSAGE_CODE_VERSION_MAJOR,
    .code_version_minor = SF_MESSAGE_CODE_VERSION_MINOR
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif




/***********************************************************************************************************************
 * Implementation of Interface
 **********************************************************************************************************************/
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
const sf_message_api_t g_sf_message_on_sf_message =
{
    .open          = SF_MESSAGE_Open,
    .close         = SF_MESSAGE_Close,
    .bufferAcquire = SF_MESSAGE_BufferAcquire,
    .bufferRelease = SF_MESSAGE_BufferRelease,
    .post          = SF_MESSAGE_Post,
    .pend          = SF_MESSAGE_Pend,
    .versionGet    = SF_MESSAGE_VersionGet
};

/*******************************************************************************************************************//**
 * @addtogroup SF_MESSAGING
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Initialize message framework. This function initiates the messaging framework control block,
 * configures the work memory corresponding to the configuration parameters.
 *
 * @retval SSP_SUCCESS          Initialization was successful.
 * @retval SSP_ERR_ASSERTION    p_ctrl, p_cfg or p_cfg->p_work_memory_start is NULL.
 * @retval SSP_ERR_INTERNAL     OS service call fails.
 * @retval SSP_ERR_IN_USE       The Messaging Framework is in use.
 * @retval SSP_ERR_INVALID_WORKBUFFER_SIZE  Invalid work buffer size.
 * @retval SSP_ERR_INVALID_MSG_BUFFER_SIZE  Message buffer size is invalid.
 * @retval SSP_ERR_ILLEGAL_SUBSCRIBER_LISTS Message subscriber lists is illegal.
 * @note   This API function is allowed to be called once per instance. The behavior if called twice is undefined.
 * @note   This API function only allows to be called from thread context.
 **********************************************************************************************************************/
ssp_err_t SF_MESSAGE_Open (sf_message_ctrl_t * const p_api_ctrl, sf_message_cfg_t const * const p_cfg)
{
    sf_message_instance_ctrl_t * p_ctrl = (sf_message_instance_ctrl_t *) p_api_ctrl;
    ssp_err_t error;

#if (SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE)
    error = sf_message_open_param_check(p_ctrl, p_cfg);
    SF_MESSAGE_ERROR_RETURN(SSP_SUCCESS == error, error);
#endif

    /** Creates the memory pools in the work memory area */
    error = sf_message_memory_pool_create(p_ctrl, p_cfg);
    SF_MESSAGE_ERROR_RETURN(SSP_SUCCESS == error, error);

    /** Registers subscriber lists */
    p_ctrl->pp_subscriber_lists         = p_cfg->pp_subscriber_lists;
    p_ctrl->number_of_subscriber_groups = 0U;
    sf_message_subscriber_list_t ** pplists = p_cfg->pp_subscriber_lists;
    while (*pplists)
    {
        p_ctrl->number_of_subscriber_groups++;
        pplists++;
    }

    /** Changes the messaging framework status from CLOSED to OPENED */
    p_ctrl->state = SF_MESSAGE_STATE_OPENED;

    return SSP_SUCCESS;
}  /* End of function SF_MESSAGE_Open() */

/*******************************************************************************************************************//**
 * @brief Closes message framework.
 *
 * @retval SSP_SUCCESS                         Successful close.
 * @retval SSP_ERR_ASSERTION                   p_ctrl is NULL.
 * @retval SSP_ERR_NOT_OPEN                    Message framework module has yet to be opened.
 * @retval SSP_ERR_ILLEGAL_SUBSCRIBER_LISTS    Message subscriber lists is illegal.
 * @note This API function only allows to be called from thread context.
 **********************************************************************************************************************/
ssp_err_t SF_MESSAGE_Close (sf_message_ctrl_t * const p_api_ctrl)
{
    sf_message_instance_ctrl_t * p_ctrl = (sf_message_instance_ctrl_t *) p_api_ctrl;

#if (SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(p_ctrl != NULL);
    SF_MESSAGE_ERROR_RETURN(SF_MESSAGE_STATE_OPENED == p_ctrl->state, SSP_ERR_NOT_OPEN);
#endif

    sf_message_subscriber_list_t ** pp_subscriber_list = p_ctrl->pp_subscriber_lists;
    uint32_t i = 0;

    /** Finds subscribers and flushes their queues */
    while ((i++) < p_ctrl->number_of_subscriber_groups)
    {
        SF_MESSAGE_ERROR_RETURN(NULL != (*pp_subscriber_list), SSP_ERR_ILLEGAL_SUBSCRIBER_LISTS);

        uint32_t j = 0;

        /* Selects a subscriber group */
        sf_message_subscriber_t ** ppsubscriber =
            (sf_message_subscriber_t **) (((*pp_subscriber_list)->pp_subscriber_group));

        while ((j++) < (*pp_subscriber_list)->number_of_nodes)
        {
            /* Selects a subscriber in the subscriber group */
            sf_message_subscriber_t * psubscriber = *ppsubscriber++;

            /* Return code not checked here as queue flush call cannot fail
             * when called with successfully created RTOS objects */
            tx_queue_flush(psubscriber->p_queue);
        }

        /* Selects next subscriber group for an event class */
        pp_subscriber_list++;
    }

    /** Deletes memory pools allocated in the work memory */
    /* The return codes are not checked here because delete calls cannot fail
     * when called with successfully created RTOS objects */
    tx_block_pool_delete(&(p_ctrl->block_pool));

    p_ctrl->state = SF_MESSAGE_STATE_CLOSED;

    return SSP_SUCCESS;
}  /* End of function SF_MESSAGE_Close() */

/*******************************************************************************************************************//**
 * @brief  Acquire buffer for message passing from the block.
 *
 * @retval SSP_SUCCESS            Buffer acquisition was successful.
 * @retval SSP_ERR_ASSERTION      p_ctrl, p_acquire_cfg or pp_buffer is NULL.
 * @retval SSP_ERR_NOT_OPEN       Message framework module has yet to be opened.
 * @retval SSP_ERR_NO_MORE_BUFFER No more buffer found in the memory block pool.
 * @retval SSP_ERR_TIMEOUT        OS service call returns timeout.
 * @retval SSP_ERR_INTERNAL       OS service call fails.
 * @note   This API function allows to be called from not only thread but also ISR.
 **********************************************************************************************************************/
ssp_err_t SF_MESSAGE_BufferAcquire (sf_message_ctrl_t const * const        p_api_ctrl,
                                    sf_message_header_t                    ** pp_buffer,
                                    sf_message_acquire_cfg_t const * const p_acquire_cfg,
                                    uint32_t const                         wait_option)
{
    sf_message_instance_ctrl_t * p_ctrl = (sf_message_instance_ctrl_t *) p_api_ctrl;

    sf_message_header_t * pblock = NULL;
    uint32_t            error;

#if (SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(p_ctrl != NULL);
    SSP_ASSERT(pp_buffer != NULL);
    SSP_ASSERT(p_acquire_cfg != NULL);
    SF_MESSAGE_ERROR_RETURN(SF_MESSAGE_STATE_OPENED == p_ctrl->state, SSP_ERR_NOT_OPEN);
#endif

    *pp_buffer = NULL;

    /** Allocates buffer in the block memory pool. */
    error = tx_block_allocate((TX_BLOCK_POOL *) &p_ctrl->block_pool, (void **) &pblock, wait_option);
    if (TX_SUCCESS == error)
    {
        sf_message_buffer_ctrl_t * pbuffer_ctrl =
            (sf_message_buffer_ctrl_t *) ((uint32_t) pblock + p_ctrl->buffer_size);

        /** Clears buffer control block */
        memset(pbuffer_ctrl, 0x0, sizeof(sf_message_buffer_ctrl_t));

        /** Sets the buffer in-use flag */
        pbuffer_ctrl->flag_b.in_use = 1U;

        /** Sets the address of the allocated buffer to 'pp_buffer' */
        *pp_buffer = pblock;

        /** Clears the event class and event code in the buffer. This is because the initial value in the buffer
         *    control block is unknown and it would not be safe. */
        (*pp_buffer)->event = 0U;

        /** Sets the 'buffer_keep' flag in the buffer control block if SF_MESSAGE_ACQUIRE_OPTION_KEEP is set to the
         *  'option' argument */
        if (p_acquire_cfg->buffer_keep)
        {
            pbuffer_ctrl->flag_b.buffer_keep = 1U;
        }
    }
    else if (TX_NO_MEMORY == error)
    {
        SSP_ERROR_LOG(SSP_ERR_NO_MORE_BUFFER, &g_module_name[0], &module_version);
        return SSP_ERR_NO_MORE_BUFFER;
    }
    else if (TX_WAIT_ERROR == error)
    {
        SSP_ERROR_LOG(SSP_ERR_TIMEOUT, &g_module_name[0], &module_version);
        return SSP_ERR_TIMEOUT;
    }
    else
    {
        SSP_ERROR_LOG(SSP_ERR_INTERNAL, &g_module_name[0], &module_version);
        return SSP_ERR_INTERNAL;
    }

    return SSP_SUCCESS;
}  /* End of function SF_MESSAGE_BufferAcquire() */

/******************************************************************************************************************//**
 * @brief  Release buffer obtained by SF_MESSAGE_BufferAcquire().
 *
 * @retval SSP_SUCCESS                       Buffer release was successful.
 * @retval SSP_ERR_NOT_OPEN                  Message framework module has yet to be opened.
 * @retval SSP_ERR_ASSERTION                 p_ctrl or p_buffer is NULL.
 * @retval SSP_ERR_ILLEGAL_BUFFER_ADDRESS    If buffer address is not aligned or p_buffer is not in the block pool range.
 * @retval SSP_ERR_BUFFER_RELEASED           Buffer has been released.
 * @retval SSP_ERR_INTERNAL                  OS service call fails
 * @note   This API function allows to be called from thread but also from ISR.
 **********************************************************************************************************************/
ssp_err_t SF_MESSAGE_BufferRelease (sf_message_ctrl_t * const         p_api_ctrl,
                                    sf_message_header_t * const       p_buffer,
                                    sf_message_release_option_t const option)
{
    uint32_t            error;
    sf_message_instance_ctrl_t * p_ctrl = (sf_message_instance_ctrl_t *) p_api_ctrl;
#if SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE
    ssp_err_t rtn_error = SSP_SUCCESS;
    rtn_error = sf_message_parameter_check(p_ctrl, p_buffer);
    SF_MESSAGE_ERROR_RETURN(SSP_SUCCESS == rtn_error, rtn_error);
#endif

    /** Calculates the address of the buffer control block */
    sf_message_buffer_ctrl_t * pbuffer_ctrl =
        (sf_message_buffer_ctrl_t *) ((uint32_t) p_buffer + p_ctrl->buffer_size);

    bool     do_callback       = false;
    bool     do_buffer_release = false;
    uint32_t do_nack_reply     = 0U;
    void     (* p_callback_cp)(sf_message_callback_args_t * p_arg) = NULL;

    SSP_CRITICAL_SECTION_DEFINE;
    SSP_CRITICAL_SECTION_ENTER;

    if (0UL == pbuffer_ctrl->flag_b.in_use)
    {
        SSP_CRITICAL_SECTION_EXIT;
        SSP_ERROR_LOG(SSP_ERR_BUFFER_RELEASED, &g_module_name[0], &module_version);
        return SSP_ERR_BUFFER_RELEASED;
    }

    void const * pcontext = pbuffer_ctrl->p_context;
    /*Check Release options */
    sf_message_release_options(pbuffer_ctrl, &do_callback,
                                   &do_buffer_release, &do_nack_reply,
                                   option, &p_callback_cp);

    /**  Release buffer in the condition below.
     *    (1) The counting semaphore is zero and the buffer keep option is not specified.
     *    (2) 'option' is set to SF_MESSAGE_RELEASE_OPTION_FORCED_RELEASE.
     */
    if (do_buffer_release)
    {
        *(uint32_t *) p_buffer = 0U;

        /**  Clears the flags in the buffer control block. */
        pbuffer_ctrl->flag_b.nak_response = 0U;
        pbuffer_ctrl->flag_b.in_use       = 0U;

        /**  Release the buffer using ThreadX API "tx_block_release" */
        error = tx_block_release((void *) p_buffer);
        SF_MESSAGE_ERROR_RETURN(TX_SUCCESS == error, SSP_ERR_INTERNAL);
    }

    /** Set back the backed up interrupt mask level. */
    SSP_CRITICAL_SECTION_EXIT;

    /** Invokes an user callback function if it is registered in the condition below.
     *    (1) The counting semaphore is zero.
     *    (2) 'option' is set to SF_MESSAGE_RELEASE_OPTION_FORCED_RELEASE.
     */
    if (do_callback)
    {
        sf_message_callback_args_t cb_arg;

        if (do_nack_reply)
        {
            /** Sets SF_MESSAGE_CALLBACK_EVENT_NAK if any subscribers for the message have responded NAK */
            cb_arg.event = SF_MESSAGE_CALLBACK_EVENT_NAK;
        }
        else
        {
            /** Sets SF_MESSAGE_CALLBACK_EVENT_ACK if all subscribers for the message have responded ACK */
            cb_arg.event = SF_MESSAGE_CALLBACK_EVENT_ACK;
        }

        /** Sets the pointer to the context to kept in the buffer control block */
        cb_arg.p_context = pcontext;

        /**  Invokes the registered user callback function. */
        p_callback_cp(&cb_arg);
    }

    return SSP_SUCCESS;
}  /* End of function SF_MESSAGE_BufferRelease() */

/******************************************************************************************************************//**
 * @brief  Post a message to the subscribers.
 *
 * @retval SSP_SUCCESS          Message posting was successful.
 * @retval SSP_ERR_ASSERTION    p_ctrl or p_buffer is NULL.
 * @retval SSP_ERR_NOT_OPEN     Message framework module has yet to be opened.
 * @retval SSP_ERR_NO_SUBSCRIBER_FOUND  No subscriber found.
 * @retval SSP_ERR_BUFFER_RELEASED  Buffer has been released.
 * @retval SSP_ERR_MESSAGE_QUEUE_FULL  Queue is full (Timeout occurs before sending a message if timeout option is
 *                              specified)
 * @retval SSP_ERR_ILLEGAL_BUFFER_ADDRESS    If buffer address is not aligned or p_buffer is not in the block pool range.
 * @retval SSP_ERR_INTERNAL     OS service call fails
 * @note   This API function allows to be called from not only thread but also ISR(if wait_option is TX_NO_WAIT).
 * @note   Another buffer writing to the buffer before the message read by message consumers results data overwriting.
 **********************************************************************************************************************/
ssp_err_t SF_MESSAGE_Post (sf_message_ctrl_t * const           p_api_ctrl,
                           sf_message_header_t const * const   p_buffer,
                           sf_message_post_cfg_t const * const p_post_cfg,
                           sf_message_post_err_t  * const      p_post_err,
                           uint32_t const                      wait_option)
{
    ssp_err_t rtn_error = SSP_SUCCESS;
    sf_message_instance_ctrl_t * p_ctrl = (sf_message_instance_ctrl_t *) p_api_ctrl;
#if SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE
    rtn_error = sf_message_parameter_check(p_ctrl, p_buffer );
    SF_MESSAGE_ERROR_RETURN(SSP_SUCCESS == rtn_error, rtn_error);
#endif

    sf_message_subscriber_list_t ** pp_subscriber_list = p_ctrl->pp_subscriber_lists;
    sf_message_buffer_ctrl_t     *  pbuffer_ctrl;
    sf_message_priority_t           priority           = SF_MESSAGE_PRIORITY_NORMAL;
    void const *                    pcontext           = NULL;
    void (* pcallback)(sf_message_callback_args_t * p_arg)  = NULL;

    if(p_post_cfg)
    {
       pcallback = p_post_cfg->p_callback;
       pcontext  = p_post_cfg->p_context;
       priority  = p_post_cfg->priority;
    }

    /** - Checks the number of the subscribers of specified event class */
    uint16_t number = sf_message_get_number_of_subscribers(p_ctrl, p_buffer, pp_subscriber_list);
    SF_MESSAGE_ERROR_RETURN(0 != number, SSP_ERR_NO_SUBSCRIBER_FOUND);


    /** Calculates the address of the buffer control block */
    pbuffer_ctrl = (sf_message_buffer_ctrl_t *) ((uint32_t) p_buffer + p_ctrl->buffer_size);

    SSP_CRITICAL_SECTION_DEFINE;
    SSP_CRITICAL_SECTION_ENTER;

    if (0U == pbuffer_ctrl->flag_b.in_use)
    {
        SSP_CRITICAL_SECTION_EXIT;
        SSP_ERROR_LOG(SSP_ERR_BUFFER_RELEASED, &g_module_name[0], &module_version);
        return SSP_ERR_BUFFER_RELEASED;
    }

    /** Counts up the counting semaphore in the buffer control block */
    pbuffer_ctrl->flag_b.semaphore = number;

    /** Registers user callback function and context passed from user */
    pbuffer_ctrl->p_callback = pcallback;
    pbuffer_ctrl->p_context  = pcontext;

    SSP_CRITICAL_SECTION_EXIT;

    /* send a message to the message queue*/
    rtn_error = sf_message_send(p_ctrl, p_buffer, p_post_err, wait_option,
                                pp_subscriber_list,priority, pbuffer_ctrl);


    return rtn_error;
}  /* End of function SF_MESSAGE_Post() */

/******************************************************************************************************************//**
 * @brief  Pend on a message.
 *
 * @retval SSP_SUCCESS          Message pending was successful.
 * @retval SSP_ERR_ASSERTION    p_ctrl, pp_buffer or p_queue is NULL.
 * @retval SSP_ERR_NOT_OPEN     Message framework module has yet to be opened.
 * @retval SSP_ERR_MESSAGE_QUEUE_EMPTY  Queue is empty.
 * @retval SSP_ERR_TIMEOUT      OS service call returns timeout.
 * @retval SSP_ERR_INTERNAL     OS service call fails.
 * @note   This API function allows to be called from not only thread but also ISR(if wait_option is TX_NO_WAIT).
 **********************************************************************************************************************/
ssp_err_t SF_MESSAGE_Pend (sf_message_ctrl_t const * const p_api_ctrl,
                           TX_QUEUE const * const          p_queue,
                           sf_message_header_t             ** pp_buffer,
                           uint32_t const                  wait_option)
{
    sf_message_instance_ctrl_t * p_ctrl = (sf_message_instance_ctrl_t *) p_api_ctrl;
    ssp_err_t rtn_error = SSP_SUCCESS;
    uint32_t error;

    SSP_PARAMETER_NOT_USED(p_ctrl);
#if (SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(p_ctrl != NULL);
    SSP_ASSERT(p_queue != NULL);
    SSP_ASSERT(pp_buffer != NULL);
    SF_MESSAGE_ERROR_RETURN(SF_MESSAGE_STATE_OPENED == p_ctrl->state, SSP_ERR_NOT_OPEN);
#endif

    /*LDRA_NOANALYSIS LDRA_INSPECTED below not working. */
    /* This is a false positive violation.  pp_buffer is an input parameter and should not be initialized here. */
    /*LDRA_INSPECTED 69 D */
    *pp_buffer = NULL;
    /*LDRA_ANALYSIS */

    /** Receiving message here. Receiving data is not message itself but the pointer to
     *  the buffer */
    error = tx_queue_receive((TX_QUEUE *) p_queue, (VOID *) (pp_buffer), (ULONG) wait_option);
    if (TX_SUCCESS == error)
    {
        rtn_error = SSP_SUCCESS;
    }
    else if (TX_QUEUE_EMPTY == error)
    {
        /** If there is no data in the message queue and TX_NO_WAIT is specified to wait_option,
         *  return immediately with SSP_ERR_MESSAGE_QUEUE_EMPTY error code */
        SSP_ERROR_LOG(SSP_ERR_MESSAGE_QUEUE_EMPTY, &g_module_name[0], &module_version);
        rtn_error = SSP_ERR_MESSAGE_QUEUE_EMPTY;
    }
    else if (TX_WAIT_ERROR != error)
    {
        SSP_ERROR_LOG(SSP_ERR_TIMEOUT, &g_module_name[0], &module_version);
        rtn_error = SSP_ERR_TIMEOUT;
    }
    else
    {
        SSP_ERROR_LOG(SSP_ERR_INTERNAL, &g_module_name[0], &module_version);
        rtn_error = SSP_ERR_INTERNAL;
    }

    return rtn_error;
}  /* End of function SF_MESSAGE_Pend() */

/******************************************************************************************************************//**
 *  @brief Get the version of the messaging framework. Stores version information in provided pointer.
 *
 * @retval SSP_SUCCESS           Got version number successfully.
 * @retval SSP_ERR_ASSERTION     p_version is NULL.
 **********************************************************************************************************************/
ssp_err_t SF_MESSAGE_VersionGet (ssp_version_t * const p_version)
{
#if SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_version);
#endif

    *p_version = module_version;

    return SSP_SUCCESS;
}  /* End of function SF_MESSAGE_VersionGet() */

/*******************************************************************************************************************//**
 * @} (end addtogroup SF_MESSAGING)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/
#if (SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE)

/*******************************************************************************************************************//**
 * @brief   This is the parameter checking subroutine for the SF_MESSAGE_Open API.
 * @param[in]     p_cfg   Pointer to the configuration structure for messaging framework
 * @retval  SSP_SUCCESS                      No parameter error found
 * @retval  SSP_ERR_ASSERTION                p_ctrl, p_cfg or p_cfg->p_work_memory_start is NULL.
 * @retval  SSP_ERR_IN_USE                   The Messaging Framework is in use
 * @retval  SSP_ERR_INVALID_WORKBUFFER_SIZE  Invalid work buffer size
 * @retval  SSP_ERR_INVALID_MSG_BUFFER_SIZE  Message buffer size is invalid
 * @retval  SSP_ERR_ILLEGAL_SUBSCRIBER_LISTS Message subscriber lists is illegal
 **********************************************************************************************************************/
static ssp_err_t sf_message_open_param_check (sf_message_instance_ctrl_t * const p_ctrl, sf_message_cfg_t const * const p_cfg)
{
    SSP_ASSERT(p_ctrl != NULL);
    SF_MESSAGE_ERROR_RETURN(SF_MESSAGE_STATE_OPENED != p_ctrl->state, SSP_ERR_IN_USE);
    SSP_ASSERT(p_cfg != NULL);
    SSP_ASSERT(p_cfg->p_work_memory_start != NULL);
    SF_MESSAGE_ERROR_RETURN((p_cfg->work_memory_size_bytes >
                             (p_cfg->buffer_size + sizeof(sf_message_buffer_ctrl_t) +
                              SF_MESSAGE_SIZE_FOR_UCHAR_POINTER_IN_BLOCK_POOL)), SSP_ERR_INVALID_WORKBUFFER_SIZE);
    SF_MESSAGE_ERROR_RETURN(p_cfg->buffer_size >= sizeof(sf_message_header_t), SSP_ERR_INVALID_MSG_BUFFER_SIZE);
    SF_MESSAGE_ERROR_RETURN(0 == (p_cfg->buffer_size % 4), SSP_ERR_INVALID_MSG_BUFFER_SIZE);
    SF_MESSAGE_ERROR_RETURN(p_cfg->pp_subscriber_lists != NULL, SSP_ERR_ILLEGAL_SUBSCRIBER_LISTS);

    return SSP_SUCCESS;
}  /* End of function sf_message_open_param_check() */
#endif /* if (SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE) */

/*******************************************************************************************************************//**
 * @brief   This is the memory pool creation subroutine for the SF_MESSAGE_Open API.
 * @param[in]     p_ctrl                       Pointer to the messaging control block
 * @param[in]     p_cfg                        Pointer to the configuration structure for messaging framework
 * @retval SSP_SUCCESS                         The memory byte pool and memory block pool was successfully created.
 * @retval SSP_ERR_INTERNAL                    OS service call fails
 * @retval SSP_ERR_INVALID_WORKBUFFER_SIZE     Invalid work buffer size
 * @note This function works if called from SF_MESSAGE_Open() and any other cases would not work correctly.
 **********************************************************************************************************************/
static ssp_err_t sf_message_memory_pool_create (sf_message_instance_ctrl_t * const p_ctrl,
                                                sf_message_cfg_t const * const     p_cfg)
{
    uint32_t memory_address = (uint32_t) p_cfg->p_work_memory_start;

    /** Adjusts the memory alignment to 32-bit boundary */
    uint32_t work_memory_size = p_cfg->work_memory_size_bytes;
    if (0 != ((uint32_t) memory_address % 4))
    {
        memory_address  -= (memory_address % 4);
        work_memory_size = (((work_memory_size + sizeof(uint32_t)) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);
    }

    /** Clears the work memory */
    memset((void *) memory_address, 0, work_memory_size);

    /** Gets number of buffers */
    uint32_t number_of_buffers = work_memory_size /
                                 ((p_cfg->buffer_size + sizeof(sf_message_buffer_ctrl_t)) +
                                  SF_MESSAGE_SIZE_FOR_UCHAR_POINTER_IN_BLOCK_POOL);

    SF_MESSAGE_ERROR_RETURN(number_of_buffers > 0U, SSP_ERR_INVALID_WORKBUFFER_SIZE);

    p_ctrl->buffer_size       = p_cfg->buffer_size;
    p_ctrl->number_of_buffers = number_of_buffers;

    /** Creates block pool for storing messages. Note that the buffer size seen in block pool control block is
     *  different from p_cfg->buffer_size because it also includes the size of buffer control block which is
     *  associated to the buffer */
    if (TX_SUCCESS != tx_block_pool_create((TX_BLOCK_POOL *) &(p_ctrl->block_pool),
                                           (CHAR *) p_cfg->p_block_pool_name,
                                           (p_cfg->buffer_size + sizeof(sf_message_buffer_ctrl_t)),
                                           (void *) memory_address,
                                           work_memory_size))
    {
        SSP_ERROR_LOG(SSP_ERR_INTERNAL, &g_module_name[0], &module_version);
        return SSP_ERR_INTERNAL;
    }

    return SSP_SUCCESS;
}  /* End of function sf_message_memory_pool_create() */


/******************************************************************************************************************//**
 * @brief   Subroutine for SF_MESSAGE_Post API which posts message to the message queue of subscriber.
 * @param[in]     p_queue       Pointer to the message queue.
 * @param[in]     p_buffer      Pointer to the buffer.
 * @param[in]     priority      Message priority(SF_MESSAGE_PRIORITY_NORMAL or SF_MESSAGE_PRIORITY_HIGH).
 * @param[in]     wait_option   Wait option (TX_NO_WAIT, TX_WAIT_FOREVER or numerical values).
 * @retval SSP_SUCCESS          Message posting was successful.
 * @retval SSP_ERR_MESSAGE_QUEUE_FULL  Queue is full.
 * @retval SSP_ERR_INTERNAL     OS service call fails.
 * @note This function works if called from SF_MESSAGE_Post() and any other cases would not work correctly.
 **********************************************************************************************************************/
static ssp_err_t sf_message_queue_send (TX_QUEUE const * const              p_queue,
                                        sf_message_header_t const * const   p_buffer,
                                        sf_message_priority_t const         priority,
                                        uint32_t const                      wait_option)
{
    ssp_err_t error;
    UINT      tx_error;

    if (SF_MESSAGE_PRIORITY_HIGH == priority)
    {
        /** Posts message to the front of the message queue of the target node if message priority is high */
        tx_error = tx_queue_front_send((TX_QUEUE *) p_queue, (VOID *) &p_buffer, (ULONG) wait_option);
    }
    else
    {
        /** - Posts message to the message queue of the target node if message priority is normal */
        tx_error = tx_queue_send((TX_QUEUE *) p_queue, (VOID *) &p_buffer, (ULONG) wait_option);
    }
    if (TX_SUCCESS == tx_error)
    {
        error = SSP_SUCCESS;
    }
    else if (TX_QUEUE_FULL == tx_error)
    {
        /** Returns SSP_ERR_MESSAGE_QUEUE_FULL if the message queue is full */
        error = SSP_ERR_MESSAGE_QUEUE_FULL;
    }
    else
    {
        error = SSP_ERR_INTERNAL;
    }
    return error;
}  /* End of function sf_message_queue_send() */

/*******************************************************************************************************************//**
 * @brief   Subroutine for SF_MESSAGE_Post API which looks up the subscriber list and return the number of message
 * subscribers which subscribe the event class specified in the posted message.
 * @param[in]     p_ctrl        Pointer to the messaging control block
 * @param[in]     p_buffer      Pointer to the buffer
 * @param[in]     pp_subscriber_list      Pointer to the subscriber list array.
 * @retval number The number of message consumers found in the subscriber list for the event class code
 * @note This function works if called from SF_MESSAGE_Post() and any other cases would not work correctly.
 **********************************************************************************************************************/
static uint16_t sf_message_get_number_of_subscribers (sf_message_instance_ctrl_t * const p_ctrl,
                                                      sf_message_header_t const * const  p_buffer,
                                                      sf_message_subscriber_list_t       ** pp_subscriber_list)
{
    uint16_t i      = 0;
    uint16_t number = 0;

    while ((i++) < p_ctrl->number_of_subscriber_groups)
    {
        if (NULL == (*pp_subscriber_list))
        {
            return number;
        }

        if (p_buffer->event_b.class_code == (*pp_subscriber_list)->event_class)
        {
            uint32_t j = 0;
            sf_message_subscriber_t ** ppsubscriber =
                (sf_message_subscriber_t **) (((*pp_subscriber_list)->pp_subscriber_group));

            while ((j++) < (*pp_subscriber_list)->number_of_nodes)
            {
                sf_message_subscriber_t * psubscriber = *ppsubscriber++;

                /** - Checks the class instance value in the common message header is in the range of event class
                 *    instance in the subscriber list.
                 */
                if ((p_buffer->event_b.class_instance >= psubscriber->instance_range.start) &&
                    (p_buffer->event_b.class_instance <= psubscriber->instance_range.end))
                {
                    number++;
                }
            }
        }

        pp_subscriber_list++;
    }

    return number;
}  /* End of function sf_message_get_number_of_subscribers() */

#if SF_MESSAGE_CFG_PARAM_CHECKING_ENABLE
/*******************************************************************************************************************//**
 * @brief   Subroutine for SF_MESSAGE_Post and SF_MESSAGE_Release API. Checks parameter and buffer address.
 * @param[in]     p_ctrl                     Pointer to the messaging control block
 * @param[in]     p_buffer                   Pointer to the buffer
 * @retval SSP_ERR_ASSERTION                 p_ctrl or p_buffer is NULL.
 * @retval SSP_ERR_NOT_OPEN                  Message framework module has yet be opened
 * @retval SSP_ERR_ILLEGAL_BUFFER_ADDRESS    If buffer address is not aligned or p_buffer is not in the block pool range.
 * @retval SSP_SUCCESS                       Parameter check is successful.
 **********************************************************************************************************************/
static ssp_err_t sf_message_parameter_check(sf_message_instance_ctrl_t * p_ctrl,
                                            sf_message_header_t const * const   p_buffer)
{
    SSP_ASSERT(p_ctrl != NULL);
    SSP_ASSERT(p_buffer != NULL);
    SF_MESSAGE_ERROR_RETURN(SF_MESSAGE_STATE_OPENED == p_ctrl->state, SSP_ERR_NOT_OPEN);

    uint32_t block_size = (uint32_t)(p_ctrl->buffer_size) + sizeof(sf_message_buffer_ctrl_t);
    uint32_t first_block_address = (uint32_t) (p_ctrl->block_pool.tx_block_pool_start) +
                                                SF_MESSAGE_SIZE_FOR_UCHAR_POINTER_IN_BLOCK_POOL;
    uint32_t end_block_address = (first_block_address +
                                 ((uint32_t)(p_ctrl->block_pool.tx_block_pool_total - 1)*
                                 (block_size + SF_MESSAGE_SIZE_FOR_UCHAR_POINTER_IN_BLOCK_POOL)));

    /* Checks buffer address is aligned to the memory block in the block pool */
    if (((uint32_t) p_buffer >= (first_block_address)) && ((uint32_t) p_buffer <= (end_block_address)))
    {
        if (((uint32_t) p_buffer - first_block_address) %
            (block_size + SF_MESSAGE_SIZE_FOR_UCHAR_POINTER_IN_BLOCK_POOL))
        {
            return SSP_ERR_ILLEGAL_BUFFER_ADDRESS;
        }
    }
    else
    {
        return SSP_ERR_ILLEGAL_BUFFER_ADDRESS;
    }
    return SSP_SUCCESS;
}
#endif

/******************************************************************************************************************//**
 * @brief  Subroutine for SF_MESSAGE_Post API which posts message to the message queue of subscriber.
 *
 * @param[in]      p_ctrl                     Pointer to the messaging control block.
 * @param[in]      p_buffer                   Pointer to the buffer.
 * @param[in]      p_post_err                 Pointer to the post error information structure.
 * @param[in]      wait_option                Wait option (TX_NO_WAIT, TX_WAIT_FOREVER or numerical values).
 * @param[in]      pp_subscriber_list         Pointer to message subscriber list.
 * @param[in]      priority                   Message priority(SF_MESSAGE_PRIORITY_NORMAL or SF_MESSAGE_PRIORITY_HIGH).
 * @param[out]     pbuffer_ctrl               Pointer to the buffer control block.
 * @retval SSP_SUCCESS                       Sent message to the message queue successfully.
 * @retval SSP_ERR_MESSAGE_QUEUE_FULL        Queue is full.
 * @retval SSP_ERR_INTERNAL                  OS service call fails.
 * @note   This API function allows to be called from not only thread but also ISR(if wait_option is TX_NO_WAIT).
 * @note   Another buffer writing to the buffer before the message read by message consumers results data overwriting.
 **********************************************************************************************************************/
static ssp_err_t sf_message_send(sf_message_instance_ctrl_t * p_ctrl,
                                 sf_message_header_t const * const   p_buffer,
                                 sf_message_post_err_t  * const      p_post_err,
                                 uint32_t const                      wait_option,
                                 sf_message_subscriber_list_t ** pp_subscriber_list,
                                 sf_message_priority_t          priority,
                                 sf_message_buffer_ctrl_t     * pbuffer_ctrl)
{
    /** Finds subscribers which expects the event class and send a message to the message queue */
    ssp_err_t rtn_error = SSP_SUCCESS;
    uint32_t i = 0U;
    while ((i++) < p_ctrl->number_of_subscriber_groups)
    {
        /** Compares an event class code in the common message header and an event class of the subscriber list.
         *  Skip the subscriber group if the comparison result does not match.
         */
        if (p_buffer->event_b.class_code == (*pp_subscriber_list)->event_class)
        {
            uint32_t j = 0;
            sf_message_subscriber_t ** ppsubscriber =
                    (sf_message_subscriber_t **) (((*pp_subscriber_list)->pp_subscriber_group));

            while ((j++) < (*pp_subscriber_list)->number_of_nodes)
            {
                ssp_err_t error;
                sf_message_subscriber_t * psubscriber = *ppsubscriber++;

                /** Checks the class instance value in the common message header is in the range of event class instance
                 *  in the subscriber list.
                 */
                if ((p_buffer->event_b.class_instance >= psubscriber->instance_range.start) &&
                        (p_buffer->event_b.class_instance <= psubscriber->instance_range.end))
                {
                    error = sf_message_queue_send( (TX_QUEUE *) psubscriber->p_queue, p_buffer,
                                                   priority, wait_option);
                    if (SSP_SUCCESS != error)
                    {
                        /** Counts down the counting semaphore because of messaging failed */
                        SSP_CRITICAL_SECTION_DEFINE;
                        SSP_CRITICAL_SECTION_ENTER;
                        pbuffer_ctrl->flag_b.semaphore--;
                        SSP_CRITICAL_SECTION_EXIT;

                        /** Sets the Error subscriber info  */
                        if (p_post_err)
                        {
                            p_post_err->p_queue = psubscriber->p_queue;
                        }
                        rtn_error = error;
                    }
                }
            }
        }
        pp_subscriber_list++;
    }
    return rtn_error;
}

/******************************************************************************************************************//**
 * @brief Subroutine for SF_MESSAGE_Release API which releases the buffer.
 *
 * @param[in]      pbuffer_ctrl                 Pointer to the buffer control block.
 * @param[out]     do_callback                  Flag for callback function.
 * @param[out]     do_buffer_release            Flag for buffer release.
 * @param[out]     do_nack_reply                Flag for NAK response.
 * @param[in]      option                       Message release option.
 * @param[out]     p_callback_cp                Pointer to callback function.
 * @note   This API function allows to be called from thread but also from ISR.
 **********************************************************************************************************************/
static void sf_message_release_options(sf_message_buffer_ctrl_t * pbuffer_ctrl,
                                    bool  *do_callback, bool *do_buffer_release,
                                    uint32_t *do_nack_reply,
                                    sf_message_release_option_t const option,
                                    void  (** p_callback_cp)(sf_message_callback_args_t * p_arg))
{
    if ((uint32_t)(SF_MESSAGE_RELEASE_OPTION_NAK) & (uint32_t)option)
    {
        /**  Sets NAK flag in the buffer control block if SF_MESSAGE_RELEASE_OPTION_NAK option is set.
         *    NAK flag is ORed logic for multiple subscribers. When both of SF_MESSAGE_RELEASE_OPTION_ACK and
         *    SF_MESSAGE_RELEASE_OPTION_NAK are set, SF_MESSAGE_RELEASE_OPTION_NAK is taken. */
        pbuffer_ctrl->flag_b.nak_response = 1U;
    }

    if ((uint32_t)(SF_MESSAGE_RELEASE_OPTION_FORCED_RELEASE) & (uint32_t)option)
    {
        if (pbuffer_ctrl->p_callback)
        {
            *do_callback   = true;
            *do_nack_reply = pbuffer_ctrl->flag_b.nak_response;
            *p_callback_cp = pbuffer_ctrl->p_callback;
        }

        *do_buffer_release                = true;
        pbuffer_ctrl->flag_b.semaphore   = 0U;
        pbuffer_ctrl->flag_b.buffer_keep = 0U;
    }
    else
    {
        if (pbuffer_ctrl->flag_b.semaphore)
        {
            /** Decrease the counting semaphore if it is greater than zero. */
            pbuffer_ctrl->flag_b.semaphore--;
        }

        if (0UL == pbuffer_ctrl->flag_b.semaphore)
        {
            if (pbuffer_ctrl->p_callback)
            {
                *do_callback   = true;
                *do_nack_reply = pbuffer_ctrl->flag_b.nak_response;
                *p_callback_cp = pbuffer_ctrl->p_callback;
            }

            if (0UL == pbuffer_ctrl->flag_b.buffer_keep)
            {
                *do_buffer_release = true;
            }
        }
    }
}
