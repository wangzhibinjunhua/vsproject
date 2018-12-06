package com.wzb.helloworld;
/**
 * @author wzb<wangzhibin_x@qq.com>
 * @date Jun 3, 2017 9:03:01 AM	
 */

public class Point {
	private float x;
	private float y;
	
	public Point(){
		
	}
	
	public Point(float x,float y){
		this.x=x;
		this.y=y;
	}
	
	public float getX(){
		return this.x;
	}
	
	public float getY(){
		return this.y;
	}
	
	public void setX(float x){
		this.x=x;
	}
	
	public void setY(float y){
		this.y=y;
	}
}
