/* A simple synchronous XML-RPC client program written in C, as an example of
   an Xmlrpc-c client.  This invokes the sample.add procedure that the
   Xmlrpc-c example xmlrpc_sample_add_server.c server provides.  I.e. it adds
   two numbers together, the hard way.

   This sends the RPC to the server running on the local system ("localhost"),
   HTTP Port 8080.

   This program uses the Xmlrpc-c global client, which uses the default
   client XML transport.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

#include "config.h"  /* information about this build environment */
#include "asa.h"

#define NAME "Xmlrpc-c Test Client"
#define VERSION "1.0"

/*********************************/
/*        Definitions            */
/*********************************/
#define LIBRARY_VERSION_MAJOR   0
#define LIBRARY_VERSION_MINOR   1
#define LIBRARY_VERSION_BUGFIX  2

/*********************************/
/*    Internal functions         */
/*********************************/

static void DieIfFaultOccurred (xmlrpc_env * const envP) 
{
    if (envP->fault_occurred) 
    {
        fprintf(stderr, "ERROR: %s (%d)\n",
                envP->fault_string, envP->fault_code);
        exit(1);
    }
}


/*
 * Read value with type double from XMLRPC struct.
 */
static void ReadDoubleStructField(xmlrpc_env *const env, 
                                  xmlrpc_value *const XMLRPCStruct,
                                  const char *FieldName,
                                  double *DoubleValuePtr)
{
    xmlrpc_value *TempXMLValue;
    xmlrpc_double TempXMLDoubleValue;
    
    xmlrpc_struct_find_value(env, XMLRPCStruct, FieldName, &TempXMLValue);
    
    if (TempXMLValue) 
    {
        xmlrpc_read_double(env, TempXMLValue, &TempXMLDoubleValue);
        DieIfFaultOccurred(env);
        *DoubleValuePtr = TempXMLDoubleValue;
    } 
    else
    {
        fprintf(stderr, "ERROR: Missing value in server response\n");
        *DoubleValuePtr = -1;
    }
    
    xmlrpc_DECREF(TempXMLValue);
}

/*
 * Read value with type int32 from XMLRPC struct.
 */
static void ReadIntStructField(xmlrpc_env *const env, 
                                  xmlrpc_value *const XMLRPCStruct,
                                  const char *FieldName,
                                  uint32_t *IntValuePtr)
{
    xmlrpc_value *TempXMLValue;
    xmlrpc_int TempXMLIntValue;
    
    xmlrpc_struct_find_value(env, XMLRPCStruct, FieldName, &TempXMLValue);
    
    if (TempXMLValue) 
    {
        xmlrpc_read_int(env, TempXMLValue, &TempXMLIntValue);
        DieIfFaultOccurred(env);
        *IntValuePtr = TempXMLIntValue;
    } 
    else
    {
        fprintf(stderr, "ERROR: Missing value in server response\n");
        *IntValuePtr = -1;
    }
    
    xmlrpc_DECREF(TempXMLValue);
}

/*
 * Read value with type string from XMLRPC struct.
 */
static void ReadStringStructField(xmlrpc_env *const env, 
                                  xmlrpc_value *const XMLRPCStruct,
                                  const char *FieldName,
                                  char **StrValuePtr)
{
    xmlrpc_value *TempXMLValue;
    
    xmlrpc_struct_find_value(env, XMLRPCStruct, FieldName, &TempXMLValue);
    
    if (TempXMLValue) 
    {
        xmlrpc_read_string(env, TempXMLValue, StrValuePtr);
        DieIfFaultOccurred(env);
    }
    else
    {
        fprintf(stderr, "ERROR: Missing value in server response\n");
        *StrValuePtr = NULL;
    }
    
    xmlrpc_DECREF(TempXMLValue);
}


/*********************************/
/*      Public functions         */
/*********************************/



version_t GetLibraryVersion(void)
{
    version_t Version = {LIBRARY_VERSION_MAJOR,
                         LIBRARY_VERSION_MINOR,
                         LIBRARY_VERSION_BUGFIX
                        };
    return Version;
}



status_t SetSettings(server_t *Server, asa_settings_t *Settings)
{
    // ------ Server connection
    status_t Status = ASA_OK;
    xmlrpc_env env;
    char serverUrl[128];
    sprintf(serverUrl, "http://%s:%s", Server->Host, Server->Port);
    
    /* Initialize our error-handling environment. */
    xmlrpc_env_init(&env);
    
    /* Create the global XML-RPC client object. */
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
    if (env.fault_occurred)
    {
        Status = SERVER_RESPONSE_ERROR;
        fprintf(stderr, "ERROR: %s (%d)\n",
            env.fault_string, env.fault_code);
        goto cleanup;
    }
    
    
    // ------ Prepare data
    
    char * DebugModelType;
    switch (Settings->DebugModelType)
    {
        case COMPONENT_MODEL_TYPE_RESISTOR:
            DebugModelType = "Resistor";
            break;
        case COMPONENT_MODEL_TYPE_CAPACITOR:
            DebugModelType = "Capacitor";
            break;
        default:
            DebugModelType = "";
    }

    char * TriggerMode;
    switch (Settings->TriggerMode)
    {
    case COMPONENT_TRIGGER_MANUAL:
        TriggerMode = "Manual";
        break;
    default:
        TriggerMode = "Auto";
    }
    
    xmlrpc_value * XMLSettingsStruct;
    XMLSettingsStruct = xmlrpc_build_value(&env, 
                                   "{s:d,s:d,s:d,s:d,s:s,s:d,s:i,s:i,s:i,s:s}", 
                                   "probe_signal_frequency", Settings->ProbeSignalFrequencyHz,
                                   "desc_frequency", Settings->DiscFreqHz,
                                   "max_voltage", Settings->VoltageAmplV,
                                   "max_current", Settings->MaxCurrentMA,
                                   "current_model_type", DebugModelType,
                                   "current_model_nominal", Settings->DebugModelNominal,
                                   "number_points", Settings->NumberPoints,
                                   "number_charge_points", Settings->NumberChargePoints,  
                                   "measure_flags", Settings->MeasureFlags,          
                                   "trigger_mode", TriggerMode
                                  );
    
    // ------ Calling procidure

    xmlrpc_value * XMLResult;
    const char * const methodName = "set_settings";
    XMLResult = xmlrpc_client_call(&env, serverUrl, methodName, "(S)", XMLSettingsStruct);
    if (env.fault_occurred)
    {
        Status = SERVER_RESPONSE_ERROR;
        fprintf(stderr, "ERROR: %s (%d)\n",
            env.fault_string, env.fault_code);
        goto cleanup;
    }
        
    
    // ------ Finalize session

    /* Dispose of our result value. */
    xmlrpc_DECREF(XMLResult);
    xmlrpc_DECREF(XMLSettingsStruct);
    cleanup:    
    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return Status;
}



status_t GetSettings(server_t *Server, asa_settings_t *Settings)
{
    // ------ Server connection
    status_t Status = ASA_OK;
    xmlrpc_env env;
    char serverUrl[128];
    sprintf(serverUrl, "http://%s:%s", Server->Host, Server->Port);

    /* Initialize our error-handling environment. */
    xmlrpc_env_init(&env);

    /* Create the global XML-RPC client object. */
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
    if (env.fault_occurred)
    {
        Status = SERVER_RESPONSE_ERROR;
        fprintf(stderr, "ERROR: %s (%d)\n",
            env.fault_string, env.fault_code);
        goto cleanup;
    }

    // ------ Calling procidure

    xmlrpc_value * XMLResult;
    const char * const methodName = "get_settings";

    /* Make the remote procedure call */
    XMLResult = xmlrpc_client_call(&env, serverUrl, methodName, "()");
    if (env.fault_occurred)
    {

        Status = SERVER_RESPONSE_ERROR;
        fprintf(stderr, "ERROR: %s (%d)\n",
        env.fault_string, env.fault_code);
        goto cleanup;
    }

    /* Parse result */
    ReadDoubleStructField(&env, XMLResult, "probe_signal_frequency", &Settings->ProbeSignalFrequencyHz);
    ReadDoubleStructField(&env, XMLResult, "desc_frequency", &Settings->DiscFreqHz);
    ReadDoubleStructField(&env, XMLResult, "max_voltage", &Settings->VoltageAmplV);
    ReadDoubleStructField(&env, XMLResult, "max_current", &Settings->MaxCurrentMA);
    ReadIntStructField(&env, XMLResult, "number_points", &Settings->NumberPoints);
    ReadIntStructField(&env, XMLResult, "number_charge_points", &Settings->NumberChargePoints);
    ReadIntStructField(&env, XMLResult, "measure_flags", &Settings->MeasureFlags);
    char StrValue[32];
    ReadStringStructField(&env, XMLResult, "current_model_type", StrValue);
    if (strcmp(StrValue, "Resistor") == 0)
    {
        Settings->DebugModelType = COMPONENT_MODEL_TYPE_RESISTOR;
    }
    else if (strcmp(StrValue, "Capacitor") == 0)
    {
        Settings->DebugModelType = COMPONENT_MODEL_TYPE_CAPACITOR;
    }
    else
    {
        Settings->DebugModelType = COMPONENT_MODEL_TYPE_NONE;
    }

    char StrValue_[32];
    ReadStringStructField(&env, XMLResult, "trigger_mode", StrValue_);
    if (strcmp(StrValue_, "Manual") == 0)
    {
        Settings->TriggerMode = COMPONENT_TRIGGER_MANUAL;
    }
    else
    {
        Settings->TriggerMode = COMPONENT_TRIGGER_AUTO;
    }
    ReadDoubleStructField(&env, XMLResult, "current_model_nominal", &Settings->DebugModelNominal);

    // ------ Finalize session

    /* Dispose of our result value. */
    xmlrpc_DECREF(XMLResult);

    cleanup:
    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return Status;
}



status_t GetIVCurve(server_t *Server, iv_curve_t *IVC, size_t size)
{
    // ------ Server connection
    status_t Status = ASA_OK;
    xmlrpc_env env;
    char serverUrl[128];
    sprintf(serverUrl, "http://%s:%s", Server->Host, Server->Port);

    /* Initialize our error-handling environment. */
    xmlrpc_env_init(&env);

    /* Create the global XML-RPC client object. */
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
    if (env.fault_occurred)
    {
        Status = SERVER_RESPONSE_ERROR;
        fprintf(stderr, "ERROR: %s (%d)\n",
            env.fault_string, env.fault_code);
        goto cleanup;
    }
    if (size > MAX_NUM_POINTS)
    {
        fprintf(stderr, "ERROR: Maximum of number of points less then size of curve!: %d < %d\n", MAX_NUM_POINTS, size);
        Status = SERVER_RESPONSE_ERROR;
        goto cleanup;
    }
    // ------ Calling procidure

    xmlrpc_value * XMLResult;
    xmlrpc_value * XMLCurrents;
    xmlrpc_value * XMLVoltages;
    
    const char * const methodName = "get_ivc";
    
    /* Make the remote procedure call */
    XMLResult = xmlrpc_client_call(&env, serverUrl, methodName, "()");
    if (env.fault_occurred)
    {
        Status = SERVER_RESPONSE_ERROR;
        fprintf(stderr, "ERROR: %s (%d)\n",
            env.fault_string, env.fault_code);
        goto cleanup;
    }
    
    /* Get our sum and print it out. */
    xmlrpc_struct_find_value(&env, XMLResult, "current", &XMLCurrents);
    if (env.fault_occurred)
    {
        Status = SERVER_RESPONSE_ERROR;
        fprintf(stderr, "ERROR: %s (%d)\n",
            env.fault_string, env.fault_code);
        goto cleanup;
    }
    xmlrpc_struct_find_value(&env, XMLResult, "voltage", &XMLVoltages);
    if (env.fault_occurred)
    {
        Status = SERVER_RESPONSE_ERROR;
        fprintf(stderr, "ERROR: %s (%d)\n",
            env.fault_string, env.fault_code);
        goto cleanup;
    }
    
    int NumPoints;
    NumPoints = xmlrpc_array_size(&env, XMLCurrents);
    if (NumPoints != size)
    {
        Status = SERVER_RESPONSE_ERROR;
    }
    
    NumPoints = xmlrpc_array_size(&env, XMLVoltages);
    if (NumPoints != size)
    {
        Status = SERVER_RESPONSE_ERROR;
    }
    
    if (Status == ASA_OK)
    {
        int i;
        xmlrpc_double XMLDoubleCurrent, XMLDoubleVoltage;
        xmlrpc_value *XMLCurrent, *XMLVoltage;
        for (i = 0; i < size; i++)
        {
            xmlrpc_array_read_item(&env, XMLCurrents, i, &XMLCurrent);
            xmlrpc_array_read_item(&env, XMLVoltages, i, &XMLVoltage);
            xmlrpc_read_double(&env, XMLCurrent, &XMLDoubleCurrent);
            xmlrpc_read_double(&env, XMLVoltage, &XMLDoubleVoltage);
            IVC->Currents[i] = XMLDoubleCurrent;
            IVC->Voltages[i] = XMLDoubleVoltage;
        }
        xmlrpc_DECREF(XMLCurrent);
        xmlrpc_DECREF(XMLVoltage);
    }
    
    // ------ Finalize session

    /* Dispose of our result value. */
    xmlrpc_DECREF(XMLResult);
    xmlrpc_DECREF(XMLCurrents);
    xmlrpc_DECREF(XMLVoltages);

    cleanup:
    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);

    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();
    return Status;
}


