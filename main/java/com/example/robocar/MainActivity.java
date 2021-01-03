package com.example.robocar;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;

import android.annotation.SuppressLint;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class MainActivity extends AppCompatActivity {

    ImageButton forward_button, backward_button, right_button, left_button;
    TextView throttle, acceleration, direction;
    boolean isForward = false, isBackward =  false, isLeft = false, isRight = false;
    Integer curr_speed = 0;
    FirebaseDatabase database = FirebaseDatabase.getInstance();
    DatabaseReference commandRef = database.getReference("command");
    DatabaseReference speedref = database.getReference("speed");
    DatabaseReference accref = database.getReference("acceleration");


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        forward_button = findViewById(R.id.forward_button);
        backward_button = findViewById(R.id.backward_button);
        left_button = findViewById(R.id.left_button);
        right_button = findViewById(R.id.right_button);


        throttle = findViewById(R.id.throttle);
        acceleration = findViewById(R.id.acceleration);
        direction = findViewById(R.id.direction);

        speedref.setValue(curr_speed);


        //setting onTouchListener on buttons
        forward_button.setOnTouchListener(new View.OnTouchListener(){
            @Override
            public boolean onTouch(View v, MotionEvent event){
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    isForward = true;
                    ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams)forward_button.getLayoutParams();
                    final float scale = forward_button.getContext().getResources().getDisplayMetrics().density;
                    params.width = (int)(75* scale + 0.5f);
                    params.height = (int)(135* scale + 0.5f);
                    new SendVolumeUpTask().execute('F');
                    forward_button.setLayoutParams(params);
                    return true;
                }
                if(event.getAction() == MotionEvent.ACTION_UP){
                    isForward =  false;
                    ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams)forward_button.getLayoutParams();
                    final float scale = forward_button.getContext().getResources().getDisplayMetrics().density;
                    params.width = (int)(80* scale + 0.5f);
                    params.height = (int)(142* scale + 0.5f);

                    forward_button.setLayoutParams(params);
                    return true;
                }
                return false;
            }
        });


        backward_button.setOnTouchListener(new View.OnTouchListener(){
            @Override
            public boolean onTouch(View v, MotionEvent event){
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    isBackward = true;
                    ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams)backward_button.getLayoutParams();
                    final float scale = backward_button.getContext().getResources().getDisplayMetrics().density;
                    params.width = (int)(95* scale + 0.5f);
                    params.height = (int)(78* scale + 0.5f);
                    new SendVolumeUpTask().execute('B');
                    backward_button.setLayoutParams(params);
                    return true;
                }
                if(event.getAction() == MotionEvent.ACTION_UP){
                    isBackward =  false;
                    ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams)backward_button.getLayoutParams();
                    final float scale = backward_button.getContext().getResources().getDisplayMetrics().density;
                    params.width = (int)(100* scale + 0.5f);
                    params.height = (int)(84* scale + 0.5f);

                    backward_button.setLayoutParams(params);
                    return true;
                }
                return false;
            }
        });

        left_button.setOnTouchListener(new View.OnTouchListener(){
            @Override
            public boolean onTouch(View v, MotionEvent event){
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    isLeft = true;
                    direction.setText("Left");
                    ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams)left_button.getLayoutParams();
                    final float scale = forward_button.getContext().getResources().getDisplayMetrics().density;
                    params.width = (int)(95* scale + 0.5f);
                    params.height = (int)(95* scale + 0.5f);
                    new SendVolumeUpTask().execute('L');
                    left_button.setLayoutParams(params);
                    return true;
                }
                if(event.getAction() == MotionEvent.ACTION_UP){
                    isLeft =  false;
                    direction.setText("");
                    ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams)left_button.getLayoutParams();
                    final float scale = left_button.getContext().getResources().getDisplayMetrics().density;
                    params.width = (int)(100* scale + 0.5f);
                    params.height = (int)(100* scale + 0.5f);

                    left_button.setLayoutParams(params);
                    return true;
                }
                return false;
            }
        });

        right_button.setOnTouchListener(new View.OnTouchListener(){
            @Override
            public boolean onTouch(View v, MotionEvent event){
                if(event.getAction() == MotionEvent.ACTION_DOWN){
                    isRight= true;
                    direction.setText("Right");
                    ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams)right_button.getLayoutParams();
                    final float scale = right_button.getContext().getResources().getDisplayMetrics().density;
                    params.width = (int)(95* scale + 0.5f);
                    params.height = (int)(95* scale + 0.5f);
                    new SendVolumeUpTask().execute('R');
                    right_button.setLayoutParams(params);
                    return true;
                }
                if(event.getAction() == MotionEvent.ACTION_UP){
                    isRight =  false;
                    direction.setText("");
                    ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams)right_button.getLayoutParams();
                    final float scale = right_button.getContext().getResources().getDisplayMetrics().density;
                    params.width = (int)(100* scale + 0.5f);
                    params.height = (int)(100* scale + 0.5f);

                    right_button.setLayoutParams(params);
                    return true;
                }
                return false;
            }
        });

        ValueEventListener accListener = new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                double acc = 0;
                acc = snapshot.getValue(double.class);
                acc = round(acc, 2);
                acceleration.setText("acc: " + String.valueOf(acc) + "m/s2");
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                //empty
            }

            private double round(double value, int places) {
                if(places < 0) throw new IllegalArgumentException();
                long factor = (long)Math.pow(10, places);
                value=  value*factor;
                long tmp = Math.round(value);
                return (double) tmp/factor;
            }
        };

        accref.addValueEventListener(accListener);
    }


    class SendVolumeUpTask extends AsyncTask<Character, Integer, Void>{

        @Override
        protected Void doInBackground(Character... command){
            long curr_millis = System.currentTimeMillis();
            long prev_millis = 0;

            while(isForward || isBackward || isLeft || isRight){
                curr_millis = System.currentTimeMillis();

                if(curr_millis - prev_millis >= 15){
                    updateDirection(command[0]);

                    if(curr_speed < 1025 && curr_speed >= 0 && isForward)
                        curr_speed += 5;
                    if(curr_speed > -1025 && curr_speed < 0 && isBackward)
                        curr_speed -= 5;
                    if(curr_speed <= 1025 && curr_speed >= 0 && isBackward)
                        curr_speed -= 5;
                    if(curr_speed >= -1025 && curr_speed < 0 && isForward)
                        curr_speed += 5;

                    prev_millis = curr_millis;

                    int throttle_percent = (int)(((double)curr_speed/1025)*100);
                    publishProgress(throttle_percent);
                    updateSpeed();

                }
            }
            return null;
        }

        @Override
        protected void onProgressUpdate(Integer... values){
            throttle.setText("throttle: " + String.valueOf(values[0]) + "%");
        }
        private void updateSpeed() {
            speedref.setValue(curr_speed);
        }

        private void updateDirection(Character command) {
            Log.e("msg", String.valueOf(command));
            commandRef.setValue(String.valueOf(command));
        }
    }

}