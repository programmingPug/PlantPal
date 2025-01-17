import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';
import { SoilData } from '../models/soil-data.model';
import { environment } from '../../environments/environment';
import { deviceDto } from '../models/device-dto.model';

@Injectable({
  providedIn: 'root'
})
export class SoilDataService {
  private baseUrl = environment.apiUrl;

  constructor(private http: HttpClient) { }

  getPlants(): Observable<SoilData[]> {
    return this.http.get<SoilData[]>(`${this.baseUrl}/SoilMoisture`);
  }

  // Update device nickname
  updateNickname(deviceDto: deviceDto): Observable<any> {
    const url = `${this.baseUrl}/Device/${deviceDto.id}/nickname`;
    return this.http.put(url, deviceDto);
  }

}
