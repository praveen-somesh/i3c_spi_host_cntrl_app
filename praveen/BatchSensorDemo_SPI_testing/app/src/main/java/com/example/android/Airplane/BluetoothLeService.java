/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.android.Airplane;

import android.app.KeyguardManager;
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Environment;
import android.os.IBinder;
import android.os.PowerManager;
import android.util.Log;
import android.widget.ExpandableListView;

import org.apache.http.util.EncodingUtils;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
/**
 * Service for managing connection and data communication with a GATT server hosted on a
 * given Bluetooth LE device.
 */
public class BluetoothLeService extends Service {
    // private final static String TAG = BluetoothLeService.class.getSimpleName();
    public static byte interrupt = 0;
    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;
    private String mBluetoothDeviceAddress;
    private BluetoothGatt mBluetoothGatt;
    private int mConnectionState = STATE_DISCONNECTED;
    private final static String TAG = "BLE_Service";
    public static int recv_data_len = 0;
    public static byte data_resp = 0;
    public static byte read_value = 0;
    public static int ble_state = 0;
    //sensor demo
    public static String sensors_data = "";
    public static int sens_data_upload=0;
    public static int datain_count=0;
    private boolean mImageSizeSent = false, lock_send, br_change = false;
    private final Object mLock = new Object();
    public static int count_200=0;
    public static int count_19=0;
    public static int status_read_done=0;
    public static int temp=0;
    public static String command_fea = "";
    /////////
    public static String mDeviceAddress;
    private ExpandableListView mGattServicesList;
    public static BluetoothLeService mBluetoothLeService;
    static ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =
            new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    public static boolean mConnected = false;
    static BluetoothGattCharacteristic mNotifyCharacteristic;
    //�������̹�����
    //����������
    private KeyguardManager.KeyguardLock kl = null;
    //������Դ������
    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;
    public final static String ACTION_GATT_CONNECTED =
            "com.example.bluetooth.le.ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED =
            "com.example.bluetooth.le.ACTION_GATT_DISCONNECTED";
    public final static String ACTION_GATT_SERVICES_DISCOVERED =
            "com.example.bluetooth.le.ACTION_GATT_SERVICES_DISCOVERED";
    public final static String ACTION_DATA_AVAILABLE =
            "com.example.bluetooth.le.ACTION_DATA_AVAILABLE";
    public final static String EXTRA_DATA =
            "com.example.bluetooth.le.EXTRA_DATA";
    public final static String DEVICE_DOES_NOT_SUPPORT_UART =
            "com.example.bluetooth.le.DEVICE_DOES_NOT_SUPPORT_UART";
    public static String HEART_RATE_MEASUREMENT = "0000ffe1-0000-1000-8000-00805f9b34fb";
    public static String CLIENT_CHARACTERISTIC_CONFIG = "00002902-0000-1000-8000-00805f9b34fb";
    public final static UUID UUID_HEART_RATE_MEASUREMENT =
            UUID.fromString(HEART_RATE_MEASUREMENT);
    public static final UUID TX_POWER_UUID = UUID.fromString("00001804-0000-1000-8000-00805f9b34fb");
    public static final UUID TX_POWER_LEVEL_UUID = UUID.fromString("00002a07-0000-1000-8000-00805f9b34fb");
    public static final UUID CCCD = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    public static final UUID FIRMWARE_REVISON_UUID = UUID.fromString("00002a26-0000-1000-8000-00805f9b34fb");
    public static final UUID DIS_UUID = UUID.fromString("0000180a-0000-1000-8000-00805f9b34fb");
    public static final UUID RX_SERVICE_UUID = UUID.fromString("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
    public static final UUID RX_CHAR_UUID = UUID.fromString("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
    public static final UUID TX_CHAR_UUID = UUID.fromString("6e400003-b5a3-f393-e0a9-e50e24dcca9e");

    // Implements callback methods for GATT events that the app cares about.  For example,
    // connection change and services discovered.
    private final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            String intentAction;
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                br_change=true;
                Log.i("STATE CONNECTED", "OK");
                intentAction = ACTION_GATT_CONNECTED;
                mConnectionState = STATE_CONNECTED;
                broadcastUpdate(intentAction);
                ble_state=1;
                Log.i(TAG, "Connected to GATT server.");
                //  Log.i(TAG,"STATE CONNECTED BROADCAST SENT - DISCOVERING SERVICES");
                Log.i(TAG, "Attempting to start service discovery:" +
                        mBluetoothGatt.discoverServices());
                // Attempts to discover services after successful connection.
                mBluetoothGatt.discoverServices();
                Log.i("AFTER DISCOVER SERVICES", "OK");

            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.i("STATE DISCONNECTED", "OK");
                intentAction = ACTION_GATT_DISCONNECTED;
                mConnectionState = STATE_DISCONNECTED;
                ble_state=2;
                Log.i(TAG, "Disconnected from GATT server.");
                broadcastUpdate(intentAction);
                Log.i(TAG,"STATE DISCONNECTED BROADCAST SENT");
            }
            if(mConnectionState==123334){
            }
        }
        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {

                broadcastUpdate(ACTION_GATT_SERVICES_DISCOVERED);
                //Log.i("GATT SUCCESS - SERVICES DISCOVERED", "OK");
            } else {
                Log.w(TAG, "onServicesDiscovered received: " + status);
            }
        }@Override
        public void onCharacteristicRead(BluetoothGatt gatt,
                                         BluetoothGattCharacteristic characteristic,
                                         int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt,
                                            BluetoothGattCharacteristic characteristic) {
            broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
        }
    };

    private void broadcastUpdate(final String action) {
        final Intent intent = new Intent(action);
        sendBroadcast(intent);
        Log.i("INTENT SENT", "OK");
    }

    private void broadcastUpdate(final String action,
                                 final BluetoothGattCharacteristic characteristic) {
        final Intent intent = new Intent(action);
        // This is special handling for the Heart Rate Measurement profile.  Data parsing is
        // carried out as per profile specifications:
        // http://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.heart_rate_measurement.xml
        // For all other profiles, writes the data formatted in HEX.
        if(TX_CHAR_UUID.equals(characteristic.getUuid())) {
            final byte[] data = characteristic.getValue();
            if (data != null && data.length > 0) {
                Log.e(TAG, "There are data coming in..if cond ........debug\n");
                final StringBuilder stringBuilder = new StringBuilder(data.length);
                int i=0;
                for (byte byteChar : data) {
                    stringBuilder.append(String.format("%02X ", byteChar));
                    Log.e(TAG, String.format("data[%d]%02x ", datain_count,byteChar));
                    datain_count++;
                  //  count_200++;
                    data_resp=1;
                    if(i++>=4) {
                        count_200++;
                        LoadObjFile.sensor_fifo_aiprlane_spi[LoadObjFile.buf_count_spi] = byteChar;
                        Log.e(TAG, String.format("sensor_fifo[%d]%02x ", LoadObjFile.buf_count_spi, LoadObjFile.sensor_fifo_aiprlane_spi[LoadObjFile.buf_count_spi]));
                        // datain_count++;
                        LoadObjFile.buf_count_spi++;
                    }

                    /*  if(status_read_done==1 ) {
                        if(count_19%61!=0) {
                            FileService datafile = new  FileService();
                            LoadObjFile.sensor_data[LoadObjFile.sen_data_count] = byteChar;
                            command_fea = command_fea + String.format("%02x \n", byteChar );
                          //  command_fea = command_fea +stringBuilder.toString();
                            Log.e(TAG, String.format(".......debug.....kps.....sensor_data[%d]:%02x ", LoadObjFile.sen_data_count, LoadObjFile.sensor_data[LoadObjFile.sen_data_count]));
                            datafile.saveToSDCard("sensor_buffer_spi.txt",command_fea, 1);
                            LoadObjFile.sen_data_count++;
                        }
                        count_19++;
                    }*/
                }
                Log.e(TAG, " It's mainstream now............debug");
                Log.e(TAG, String.format("%02x", data.length));
                if ((data[0] & 0x49) != 0 ) {
                    interrupt = 1;
                    Log.e(TAG, "the interrupt coming .........debug");
                }
                else{
                    data_resp = 1;
                    read_value = data[4];
                    Log.e(TAG, "the data_resp here .ffffffff...............debug");
                }
            }
        }
        else {
            // For all other profiles, writes the data formatted in HEX.
            final byte[] data=characteristic.getValue();
            if(data !=null && data.length > 0){
                Log.e(TAG, "There are data coming in..else............debug\n");
                final StringBuilder stringBuilder = new StringBuilder(data.length);
                for (byte byteChar:data){
                    stringBuilder.append(String.format("%02X ", byteChar));
                    Log.e(TAG, String.format("%02x ", byteChar));
                }
            }
        }

        sendBroadcast(intent);
    }
    public class LocalBinder extends Binder {
        BluetoothLeService getService() {
            return BluetoothLeService.this;
        }
    }
    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }
    @Override
    public boolean onUnbind(Intent intent) {
        // After using a given device, you should make sure that BluetoothGatt.close() is called
        // such that resources are cleaned up properly.  In this particular example, close() is
        // invoked when the UI is disconnected from the Service.
        close();
        return super.onUnbind(intent);
    }
    private final IBinder mBinder = new LocalBinder();

    /**
     * Initializes a reference to the local Bluetooth adapter.
     *
     * @return Return true if the initialization is successful.
     */
    public boolean initialize() {
        // For API level 18 and above, get a reference to BluetoothAdapter through
        // BluetoothManager.
        if (mBluetoothManager == null) {
            mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            if (mBluetoothManager == null) {
                Log.e(TAG, "Unable to initialize BluetoothManager.");
                return false;
            }
        }

        mBluetoothAdapter = mBluetoothManager.getAdapter();
        if (mBluetoothAdapter == null) {
            Log.e(TAG, "Unable to obtain a BluetoothAdapter.");
            return false;
        }

        return true;
    }
    /**
     * Connects to the GATT server hosted on the Bluetooth LE device.
     *
     * @param address The device address of the destination device.
     *
     * @return Return true if the connection is initiated successfully. The connection result
     *         is reported asynchronously through the
     *         {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     *         callback.
     */
    public boolean connect(final String address) {
        if (mBluetoothAdapter == null || address == null) {
            mBluetoothGatt.connect();
            Log.w(TAG, "BluetoothAdapter not initialized or unspecified address.");
            return false;
        }

        // Previously connected device.  Try to reconnect.
        if (mBluetoothDeviceAddress != null && address.equals(mBluetoothDeviceAddress)
                && mBluetoothGatt != null) {
            Log.d(TAG, "Trying to use an existing mBluetoothGatt for connection.");
            if (mBluetoothGatt.connect()) {
                mConnectionState = STATE_CONNECTING;
                return true;
            } else {
                return false;
            }
        }
        final BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        if (device == null) {
            Log.w(TAG, "Device not found.  Unable to connect.");
            return false;
        }
        // We want to directly connect to the device, so we are setting the autoConnect
        // parameter to false.
        mBluetoothGatt = device.connectGatt(this, true, mGattCallback);
        Log.d(TAG, "Trying to create a new connection.");
        mBluetoothDeviceAddress = address;
        mConnectionState = STATE_CONNECTING;
        return true;
    }
    /**
     * Disconnects an existing connection or cancel a pending connection. The disconnection result
     * is reported asynchronously through the
     * {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     * callback.
     */
    public void disconnect() {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.disconnect();
    }
    /**
     * After using a given BLE device, the app must call this method to ensure resources are
     * released properly.
     */
    public void close() {
        if (mBluetoothGatt == null) {
            return;
        }
        mBluetoothGatt.close();
        mBluetoothGatt = null;
    }
    /**
     * Request a read on a given {@code BluetoothGattCharacteristic}. The read result is reported
     * asynchronously through the {@code BluetoothGattCallback#onCharacteristicRead(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)}
     * callback.
     *
     * @param characteristic The characteristic to read from.
     */
    public void readCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.readCharacteristic(characteristic);
    }
   /* public void writeCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.writeCharacteristic(characteristic);
    }*/

    /**
     * Enables or disables notification on a give characteristic.
     *
     * @param characteristic Characteristic to act on.
     * @param enabled If true, enable notification.  False otherwise.
     */
    public void setCharacteristicNotification(BluetoothGattCharacteristic characteristic,
                                              boolean enabled) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.setCharacteristicNotification(characteristic, enabled);
// This is specific to Heart Rate Measurement.
        if (UUID_HEART_RATE_MEASUREMENT.equals(characteristic.getUuid())) {
            BluetoothGattDescriptor descriptor = characteristic.getDescriptor(
                    UUID.fromString(CLIENT_CHARACTERISTIC_CONFIG));
            descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
            mBluetoothGatt.writeDescriptor(descriptor);
        }
    }
    /**
     * Retrieves a list of supported GATT services on the connected device. This should be
     * invoked only after {@code BluetoothGatt#discoverServices()} completes successfully.
     *
     * @return A {@code List} of supported services.
     */
    public List<BluetoothGattService> getSupportedGattServices() {
        if (mBluetoothGatt == null) return null;

        return mBluetoothGatt.getServices();
    }
    public void  enableTXNotification(){
        /*
    	if (mBluetoothGatt == null) {
    		showMessage("mBluetoothGatt null" + mBluetoothGatt);
    		broadcastUpdate(DEVICE_DOES_NOT_SUPPORT_UART);
    		return;
    	}
    		*/
        BluetoothGattService RxService = mBluetoothGatt.getService(RX_SERVICE_UUID);
        if (RxService == null) {
            showMessage("Rx service not found!");
            broadcastUpdate(DEVICE_DOES_NOT_SUPPORT_UART);
            return;
        }

        BluetoothGattCharacteristic TxChar = RxService.getCharacteristic(TX_CHAR_UUID);
        if (TxChar == null) {
            showMessage("Tx charateristic not found!");
            broadcastUpdate(DEVICE_DOES_NOT_SUPPORT_UART);
            return;
        }
        mBluetoothGatt.setCharacteristicNotification(TxChar,true);

        BluetoothGattDescriptor descriptor = TxChar.getDescriptor(CCCD);
        descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
        mBluetoothGatt.writeDescriptor(descriptor);
    }

    public void writeRXCharacteristic(byte[] value){
        BluetoothGattService RxService = mBluetoothGatt.getService(RX_SERVICE_UUID);
        showMessage("mBluetoothGatt null"+ mBluetoothGatt);
        if (RxService == null) {
            showMessage("Rx service not found!");
            broadcastUpdate(DEVICE_DOES_NOT_SUPPORT_UART);
            return;
        }
        BluetoothGattCharacteristic RxChar = RxService.getCharacteristic(RX_CHAR_UUID);
        if (RxChar == null) {
            showMessage("Rx charateristic not found!");
            broadcastUpdate(DEVICE_DOES_NOT_SUPPORT_UART);
            return;
        }
        // if(br_change==true)  //lattice//
        //   {//
        //  showMessage("BR_change is true......true!");//
        //     RxChar.setValue("BR4800".getBytes());//
        //    br_change=false;//
        //  }//
        //  else//
        {
            RxChar.setValue(value);
        }
        try {
            // Log.d(TAG, "thread sleep here for 1ms");
            Thread.sleep(1);
        } catch (InterruptedException e){
            e.printStackTrace();
        }
        boolean status = mBluetoothGatt.writeCharacteristic(RxChar);

        Log.d(TAG, "write TXchar - status=" + status);
    }
    private void showMessage(String msg) {
        Log.e(TAG, msg);
    }



    static public class FileService {

        private static final Boolean TRUE = null;
        private static final Boolean FALSE = null;



        public void saveToSDCard(String name, String content, int new_flag) {

            FileOutputStream fos = null;

            try{

                //Environment.getExternalStorageDirectory()����ȡsd����·��
                File file = new File(Environment.getExternalStorageDirectory(),name);

                if ( new_flag == 1) {
                    fos = new FileOutputStream(file, false);
                }else{
                    fos = new FileOutputStream(file, true);
                }

                fos.write(content.getBytes());
//    	    fos.write("\n".getBytes());
            }catch(Exception e){

                e.printStackTrace();

            }finally{

                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        public void saveToRom(String name, String content) {
            // TODO Auto-generated method stub

        }

        public Boolean existfile(String name) {
            File file = new File(Environment.getExternalStorageDirectory(),name);
            if( !file.exists()) {
                return TRUE;
            }else{
                return FALSE;
            }

        }



        public String readSDFile(String fileName) throws IOException {

            File file = new File(Environment.getExternalStorageDirectory(),fileName);
            FileInputStream fis = new FileInputStream(file);

            int length = fis.available();

            byte [] buffer = new byte[length];
            fis.read(buffer);


            String res = EncodingUtils.getString(buffer, "UTF-8");
            // file.deleteOnExit();//////////////////////////12 oct
            // command2_fea.isEmpty();/////////////////////// 12
            // command3_fea.isEmpty();/////////////////////// 12
            //      Log.e(TAG, res);

            //      for(String bytechar:  res.split(" ") ) {
            //      	Log.e(TAG, bytechar );
            //      }

            fis.close();
            // add below line 28 sept
            // MainActivity.reg_wr_byte((byte) 0x00,(byte) 0x00, (byte) 0x80 );
            return res;

        }

    }


}

