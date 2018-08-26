#include <cmath>
#include <iostream>
#include <vector>

#include "DepthFirstSearch.hpp"
#include "Path.hpp"

using namespace std;

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/offset_ptr.hpp>

using namespace boost::interprocess;


#define NOME_DA_MEMORIA2 "memoria2"


//variaveis robo
#define VEL_MOT 4.25 //tecnicamente em rad/s
#define DIS_RETO 0.05 //tamanho do passo em m ate inicio da leitura dos sensores
#define DIS_CURVA 0.262 //delta para virar PI, ja que ha o delay de comunicacao e eh float, nunca igual, (~VEL_MOT/(7.5PI)) - aleatorio
#define D_ANG 0.1 //delta para captacao de salto da atan, (~DIS_CURVA-0.1)
#define MAX_INTE 0.2 //delta para media ta intensidade. tecnicamente, deve ser zero


extern "C" {
#include "remoteApi/extApi.h"
#include "remoteApi/extApiPlatform.h"
}

int current_command = 0;
vector<int> commands;

void set_commands(const std::vector<int> &);
int get_next_command();

int main(int argc, char **argv)
{
	PathFinder *finder = new  DepthFirstSearch(State::getStartState(), State::getEndState());
	set_commands(Path::getPath(finder));
	delete (finder);

	string serverIP = "127.0.0.1";
	int serverPort = 19999;
	int clientID = simxStart((simxChar *) serverIP.c_str(), serverPort, true, true, 2000, 5);

	//inteiros para localizacao dos objetos no vrep
	//inteiros para partes moveis
	int leftMotorHandle = 0, rightMotorHandle = 0, bubbleRob = 0, turnleft = 0, turnright = 0;

	//inteiros para sensores de visao
	int Left_Vision_sensor = 0, LM_Vision_sensor = 0, Middle_Vision_sensor = 0, RM_Vision_sensor = 0, Right_Vision_sensor = 0;

	//inteiros para sensores ultrassom e os objetos detectados
	int Left_ultrasonic = 0, LM_ultrasonic = 0, Middle_ultrasonic = 0, RM_ultrasonic = 0, Right_ultrasonic = 0;
	int detectedObjetHandleMU = 0; //detectedObjetHandleLU = 0, detectedObjetHandleLMU = 0, detectedObjetHandleRMU = 0, detectedObjetHandleRU = 0;

	//variaveis para movimentacao do robo
	float vLeft = 0, vRight = 0, dx = 0, dy = 0, X_inicial = 0, Y_inicial = 0, d_ang = 0, ang_inicial = 0, ang = 0;

	//vetores para captacao da posicao na cena
	float *angle = new float[3];
	float *position = new float[3];

	//variaveis sensores ultrassom
	float *detectedObjetMU = new float[3];//coordenadas do objeto identificado (referencial do sensor). A última posicao possui o valor mais revelante
	float *detectedSurfaceMU = new float[3];//coordenadas da superficie identificada (referencial do sensor)
	unsigned char readingMU = 0;


	//variaveis sensores de visao
	unsigned char readingLVS = 0;//leitura do sensor de visao
	float *DataLVS = new float[15];//dados da leitura
	int *auxLVS;//vetor auxiliar para casos especificos
	unsigned char readingLMVS = 0;
	float *DataLMVS = new float[15];
	int *auxLMVS;
	unsigned char readingMVS = 0;
	float *DataMVS = new float[15];
	int *auxMVS;
	unsigned char readingRMVS = 0;
	float *DataRMVS = new float[15];
	int *auxRMVS;
	unsigned char readingRVS = 0;
	float *DataRVS = new float[15];
	int *auxRVS;

	///conexao com elementos da cena
	if (clientID != -1) {
		cout << "Servidor conectado!" << std::endl;

		// inicialização dos motores e do robo
		if (simxGetObjectHandle(clientID, "bubbleRob", &bubbleRob, simx_opmode_oneshot_wait) == simx_return_ok)
			cout << "conectado ao bubbleRob" << endl;
		if (simxGetObjectHandle(clientID, "Left_ultrasonic", &Left_ultrasonic, simx_opmode_oneshot_wait) ==
			simx_return_ok &&
			simxGetObjectHandle(clientID, "LM_ultrasonic", &LM_ultrasonic, simx_opmode_oneshot_wait) ==
			simx_return_ok &&
			simxGetObjectHandle(clientID, "Middle_ultrasonic", &Middle_ultrasonic, simx_opmode_oneshot_wait) ==
			simx_return_ok &&
			simxGetObjectHandle(clientID, "RM_ultrasonic", &RM_ultrasonic, simx_opmode_oneshot_wait) ==
			simx_return_ok &&
			simxGetObjectHandle(clientID, "Right_ultrasonic", &Right_ultrasonic, simx_opmode_oneshot_wait) ==
			simx_return_ok)
			cout << "conectado aos sensores ultrassom" << endl;
		if (simxGetObjectHandle(clientID, "Left_Vision_sensor", &Left_Vision_sensor, simx_opmode_oneshot_wait) ==
			simx_return_ok &&
			simxGetObjectHandle(clientID, "LM_Vision_sensor", &LM_Vision_sensor, simx_opmode_oneshot_wait) ==
			simx_return_ok &&
			simxGetObjectHandle(clientID, "Middle_Vision_sensor", &Middle_Vision_sensor, simx_opmode_oneshot_wait) ==
			simx_return_ok &&
			simxGetObjectHandle(clientID, "RM_Vision_sensor", &RM_Vision_sensor, simx_opmode_oneshot_wait) ==
			simx_return_ok &&
			simxGetObjectHandle(clientID, "Right_Vision_sensor", &Right_Vision_sensor, simx_opmode_oneshot_wait) ==
			simx_return_ok
				)
			cout << "conectado aos sensores de visao" << endl;

		if (simxGetObjectHandle(clientID, (const simxChar *) "bubbleRob_leftMotor", (simxInt *) &leftMotorHandle,
								(simxInt) simx_opmode_oneshot_wait) != simx_return_ok)
			cout << "Handle do motor esquerdo nao encontrado!" << std::endl;
		else
			cout << "Conectado ao motor esquerdo!" << std::endl;

		if (simxGetObjectHandle(clientID, (const simxChar *) "bubbleRob_rightMotor", (simxInt *) &rightMotorHandle,
								(simxInt) simx_opmode_oneshot_wait) != simx_return_ok)
			cout << "Handle do motor direito nao encontrado!" << std::endl;
		else
			cout << "Conectado ao motor direito!" << std::endl;
		simxGetObjectOrientation(clientID, bubbleRob, -1, angle,
								 simx_opmode_streaming);//alpha, beta e gamma. Usa-se o gamma
		simxGetObjectPosition(clientID, bubbleRob, -1, position, simx_opmode_streaming);//x, y, z. Nao usa-se o z
		simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU, &detectedObjetHandleMU,
								detectedSurfaceMU, simx_opmode_streaming);//apenas ultrassom central esta sendo usado
		simxReadVisionSensor(clientID, Left_Vision_sensor, &readingLVS, &DataLVS, &auxLVS, simx_opmode_streaming);
		simxReadVisionSensor(clientID, LM_Vision_sensor, &readingLMVS, &DataLMVS, &auxLMVS, simx_opmode_streaming);
		simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS, simx_opmode_streaming);
		simxReadVisionSensor(clientID, RM_Vision_sensor, &readingRMVS, &DataRMVS, &auxRMVS, simx_opmode_streaming);
		simxReadVisionSensor(clientID, Right_Vision_sensor, &readingRVS, &DataRVS, &auxRVS, simx_opmode_streaming);
	}

	//loop de execucao
	while (simxGetConnectionId(clientID) != -1) {
//        pair<int *, managed_shared_memory::size_type> comando1;
//        comando1 = abrindo_memoria->find<int>(NOME_DO_INT_NA_MEMORIA1);

		int comando = get_next_command();
		cout << "Comando:" << comando << endl;

		simxGetObjectPosition(clientID, bubbleRob, -1, position, simx_opmode_buffer);
		simxGetObjectOrientation(clientID, bubbleRob, -1, angle, simx_opmode_buffer);
		X_inicial = position[0];
		Y_inicial = position[1];
		ang_inicial = angle[2];//virar para esquerda, ang aumenta. direita, diminui
		simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU, &detectedObjetHandleMU,
								detectedSurfaceMU, simx_opmode_buffer);
		simxReadVisionSensor(clientID, Left_Vision_sensor, &readingLVS, &DataLVS, &auxLVS, simx_opmode_buffer);
		simxReadVisionSensor(clientID, LM_Vision_sensor, &readingLMVS, &DataLMVS, &auxLMVS, simx_opmode_buffer);
		simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS, simx_opmode_buffer);
		simxReadVisionSensor(clientID, RM_Vision_sensor, &readingRMVS, &DataRMVS, &auxRMVS, simx_opmode_buffer);
		simxReadVisionSensor(clientID, Right_Vision_sensor, &readingRVS, &DataRVS, &auxRVS, simx_opmode_buffer);
		readingLVS = DataLVS[10] < MAX_INTE;//media da intensidade. No caso preto, sao todos 0
		readingLMVS = DataLMVS[10] < MAX_INTE;
		readingMVS = DataMVS[10] < MAX_INTE;
		readingRMVS = DataRMVS[10] < MAX_INTE;
		readingRVS = DataRVS[10] < MAX_INTE;

		//linha reta (se nao estiver perto de um obstaculo)
		if (comando == 8 && readingMU == 0) {
			/*while((dx < DIS_RETO && dy <DIS_RETO) && readingMU==0)//escapar do estado de verificacao dos sensores
			{
				vLeft = VEL_MOT;
				vRight = VEL_MOT;
				simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
				simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
				simxGetObjectPosition(clientID, bubbleRob, -1, position, simx_opmode_buffer);
				simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU, &detectedObjetHandleMU, detectedSurfaceMU, simx_opmode_buffer);
				dx = abs(position[0]-X_inicial);
				dy = abs(position[1]-Y_inicial);
			}*/
			while (((readingLVS && readingMVS) || (readingMVS && readingRVS)) &&
				   readingMU == 0)//verificar se ta no mesmo estado, tecnicamente inutil devido a funcao acima
			{
				vLeft = VEL_MOT;
				vRight = VEL_MOT;
				simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
				simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
				simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU,
										&detectedObjetHandleMU, detectedSurfaceMU, simx_opmode_buffer);
				simxReadVisionSensor(clientID, Left_Vision_sensor, &readingLVS, &DataLVS, &auxLVS, simx_opmode_buffer);
				simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
									 simx_opmode_buffer);
				simxReadVisionSensor(clientID, Right_Vision_sensor, &readingRVS, &DataRVS, &auxRVS, simx_opmode_buffer);
				readingLVS = DataLVS[10] < MAX_INTE;//media da intensidade. No caso preto, sao todos 0
				readingMVS = DataMVS[10] < MAX_INTE;
				readingRVS = DataRVS[10] < MAX_INTE;
				extApi_sleepMs(5);
				if (readingMU) {
					cout << "objeto " << detectedObjetHandleMU << " na posicao " << detectedObjetMU[0] << ", "
						 << detectedObjetMU[1] << ", " << detectedObjetMU[2] << endl;
					cout << "superficie em " << detectedSurfaceMU[0] << ", " << detectedSurfaceMU[1] << ", "
						 << detectedSurfaceMU[2] << endl;
				}
			}
			while (!((readingLVS && readingMVS) || (readingMVS && readingRVS)) &&
				   readingMU == 0)//anda ate os sensores captarem a linha ou obstaculo
			{
				vLeft = VEL_MOT;
				vRight = VEL_MOT;
				simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
				simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
				simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU,
										&detectedObjetHandleMU, detectedSurfaceMU, simx_opmode_buffer);
				simxReadVisionSensor(clientID, Left_Vision_sensor, &readingLVS, &DataLVS, &auxLVS, simx_opmode_buffer);
				simxReadVisionSensor(clientID, LM_Vision_sensor, &readingLMVS, &DataLMVS, &auxLMVS, simx_opmode_buffer);
				simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
									 simx_opmode_buffer);
				simxReadVisionSensor(clientID, RM_Vision_sensor, &readingRMVS, &DataRMVS, &auxRMVS, simx_opmode_buffer);
				simxReadVisionSensor(clientID, Right_Vision_sensor, &readingRVS, &DataRVS, &auxRVS, simx_opmode_buffer);
				readingLVS = DataLVS[10] < MAX_INTE;//media da intensidade. No caso preto, sao todos 0
				readingLMVS = DataLMVS[10] < MAX_INTE;
				readingMVS = DataMVS[10] < MAX_INTE;
				readingRMVS = DataRMVS[10] < MAX_INTE;
				readingRVS = DataRVS[10] < MAX_INTE;
				//if(!readingMVS&&(readingRMVS||readingRVS))//robo esta desviando para a esquerda, gira direita
				if (readingRVS) {
					vLeft = VEL_MOT;
					vRight = VEL_MOT / 4;
					simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
					simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
					simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU,
											&detectedObjetHandleMU, detectedSurfaceMU, simx_opmode_buffer);
					simxReadVisionSensor(clientID, Left_Vision_sensor, &readingLVS, &DataLVS, &auxLVS,
										 simx_opmode_buffer);
					simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
										 simx_opmode_buffer);
					simxReadVisionSensor(clientID, Right_Vision_sensor, &readingRVS, &DataRVS, &auxRVS,
										 simx_opmode_buffer);
					readingLVS = DataLVS[10] < MAX_INTE;//media da intensidade. No caso preto, sao todos 0
					readingMVS = DataMVS[10] < MAX_INTE;
					readingRVS = DataRVS[10] < MAX_INTE;
					turnright = 1;
					turnleft = 0;
				} else if (readingRMVS) {
					vLeft = VEL_MOT;
					vRight = VEL_MOT / 2;
					simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
					simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
					simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU,
											&detectedObjetHandleMU, detectedSurfaceMU, simx_opmode_buffer);
					simxReadVisionSensor(clientID, Left_Vision_sensor, &readingLVS, &DataLVS, &auxLVS,
										 simx_opmode_buffer);
					simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
										 simx_opmode_buffer);
					simxReadVisionSensor(clientID, Right_Vision_sensor, &readingRVS, &DataRVS, &auxRVS,
										 simx_opmode_buffer);
					readingLVS = DataLVS[10] < MAX_INTE;//media da intensidade. No caso preto, sao todos 0
					readingMVS = DataMVS[10] < MAX_INTE;
					readingRVS = DataRVS[10] < MAX_INTE;
					turnright = 1;
					turnleft = 0;
				} else if (readingLMVS)//robo ta desviando para a direita, gira esquerda
				{
					vLeft = VEL_MOT / 2;
					vRight = VEL_MOT;
					simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
					simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
					simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU,
											&detectedObjetHandleMU, detectedSurfaceMU, simx_opmode_buffer);
					simxReadVisionSensor(clientID, Left_Vision_sensor, &readingLVS, &DataLVS, &auxLVS,
										 simx_opmode_buffer);
					simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
										 simx_opmode_buffer);
					simxReadVisionSensor(clientID, Right_Vision_sensor, &readingRVS, &DataRVS, &auxRVS,
										 simx_opmode_buffer);
					readingLVS = DataLVS[10] < MAX_INTE;//media da intensidade. No caso preto, sao todos 0
					readingMVS = DataMVS[10] < MAX_INTE;
					readingRVS = DataRVS[10] < MAX_INTE;
					turnright = 0;
					turnleft = 1;
				} else if (readingLVS)//robo ta desviando para a direita, gira esquerda
				{
					vLeft = VEL_MOT / 4;
					vRight = VEL_MOT;
					simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
					simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
					simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU,
											&detectedObjetHandleMU, detectedSurfaceMU, simx_opmode_buffer);
					simxReadVisionSensor(clientID, Left_Vision_sensor, &readingLVS, &DataLVS, &auxLVS,
										 simx_opmode_buffer);
					simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
										 simx_opmode_buffer);
					simxReadVisionSensor(clientID, Right_Vision_sensor, &readingRVS, &DataRVS, &auxRVS,
										 simx_opmode_buffer);
					readingLVS = DataLVS[10] < MAX_INTE;//media da intensidade. No caso preto, sao todos 0
					readingMVS = DataMVS[10] < MAX_INTE;
					readingRVS = DataRVS[10] < MAX_INTE;
					turnright = 0;
					turnleft = 1;
				} else if (!(readingLVS || readingLMVS || readingMVS || readingRMVS || readingRMVS)) {
					if (turnright) {
						vLeft = VEL_MOT;
						vRight = VEL_MOT / 4;
						simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
						simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight,
												   simx_opmode_streaming);
						simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU,
												&detectedObjetHandleMU, detectedSurfaceMU, simx_opmode_buffer);
						simxReadVisionSensor(clientID, Left_Vision_sensor, &readingLVS, &DataLVS, &auxLVS,
											 simx_opmode_buffer);
						simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
											 simx_opmode_buffer);
						simxReadVisionSensor(clientID, Right_Vision_sensor, &readingRVS, &DataRVS, &auxRVS,
											 simx_opmode_buffer);
						readingLVS = DataLVS[10] < MAX_INTE;//media da intensidade. No caso preto, sao todos 0
						readingMVS = DataMVS[10] < MAX_INTE;
						readingRVS = DataRVS[10] < MAX_INTE;
					} else if (turnleft) {
						vLeft = VEL_MOT / 4;
						vRight = VEL_MOT;
						simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
						simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight,
												   simx_opmode_streaming);
						simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU,
												&detectedObjetHandleMU, detectedSurfaceMU, simx_opmode_buffer);
						simxReadVisionSensor(clientID, Left_Vision_sensor, &readingLVS, &DataLVS, &auxLVS,
											 simx_opmode_buffer);
						simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
											 simx_opmode_buffer);
						simxReadVisionSensor(clientID, Right_Vision_sensor, &readingRVS, &DataRVS, &auxRVS,
											 simx_opmode_buffer);
						readingLVS = DataLVS[10] < MAX_INTE;//media da intensidade. No caso preto, sao todos 0
						readingMVS = DataMVS[10] < MAX_INTE;
						readingRVS = DataRVS[10] < MAX_INTE;
					}
				}
				if (readingMU) {
					cout << "objeto " << detectedObjetHandleMU << " na posicao " << detectedObjetMU[0] << ", "
						 << detectedObjetMU[1] << ", " << detectedObjetMU[2] << endl;
					cout << "superficie em " << detectedSurfaceMU[0] << ", " << detectedSurfaceMU[1] << ", "
						 << detectedSurfaceMU[2] << endl;
				}
				extApi_sleepMs(5);
			}
			simxGetObjectPosition(clientID, bubbleRob, -1, position, simx_opmode_buffer);
			X_inicial = position[0];
			Y_inicial = position[1];
			dx = 0;
			dy = 0;
			while ((dx < DIS_RETO && dy < DIS_RETO) && readingMU == 0)//chegar na linha
			{
				vLeft = VEL_MOT;
				vRight = VEL_MOT;
				simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
				simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
				simxGetObjectPosition(clientID, bubbleRob, -1, position, simx_opmode_buffer);
				simxReadProximitySensor(clientID, Middle_ultrasonic, &readingMU, detectedObjetMU,
										&detectedObjetHandleMU, detectedSurfaceMU, simx_opmode_buffer);
				dx = abs(position[0] - X_inicial);
				dy = abs(position[1] - Y_inicial);
				extApi_sleepMs(5);
				if (readingMU) {
					cout << "objeto " << detectedObjetHandleMU << " na posicao " << detectedObjetMU[0] << ", "
						 << detectedObjetMU[1] << ", " << detectedObjetMU[2] << endl;
					cout << "superficie em " << detectedSurfaceMU[0] << ", " << detectedSurfaceMU[1] << ", "
						 << detectedSurfaceMU[2] << endl;
				}
			}
		}

			//virar direita
		else if (comando == 6) {
			while (d_ang < (M_PI / 4 - DIS_CURVA))//escapar do estado de verificacao dos sensores
			{
				vLeft = VEL_MOT / 1.5;
				vRight = -VEL_MOT / 1.5;
				simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
				simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
				simxGetObjectOrientation(clientID, bubbleRob, -1, angle, simx_opmode_buffer);
				ang = (angle[2]);
				d_ang = abs(ang - ang_inicial);
				//se houve salto da atan, ignora colocando o salto na posicao normal. Como abs, mas funciona melhor
				if (d_ang > (M_PI / 2 - D_ANG)) {
					ang = -ang;
					d_ang = abs(ang - ang_inicial);
				}
				extApi_sleepMs(5);
			}
			simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS, simx_opmode_buffer);
			simxReadVisionSensor(clientID, RM_Vision_sensor, &readingRMVS, &DataRMVS, &auxRMVS, simx_opmode_buffer);
			readingMVS = DataMVS[10] < MAX_INTE;
			readingRMVS = DataRMVS[10] < MAX_INTE;
			while (readingMVS)//verificar se ta no mesmo estado, tecnicamente inutil devido a funcao acima
			{
				vLeft = VEL_MOT / 1.5;
				vRight = -VEL_MOT / 1.5;
				simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
				simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
				simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
									 simx_opmode_buffer);
				simxReadVisionSensor(clientID, RM_Vision_sensor, &readingRMVS, &DataRMVS, &auxRMVS, simx_opmode_buffer);
				readingMVS = DataMVS[10] < MAX_INTE;
				readingRMVS = DataRMVS[10] < MAX_INTE;
				extApi_sleepMs(5);
			}
			while (!(readingMVS))//gira ate os sensores captarem a linha
			{
				vLeft = VEL_MOT / 1.5;
				vRight = -VEL_MOT / 1.5;
				simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
				simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
				simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
									 simx_opmode_buffer);
				simxReadVisionSensor(clientID, RM_Vision_sensor, &readingRMVS, &DataRMVS, &auxRMVS, simx_opmode_buffer);
				readingMVS = DataMVS[10] < MAX_INTE;
				readingRMVS = DataRMVS[10] < MAX_INTE;
				extApi_sleepMs(5);
			}
		}

			//virar esquerda
		else if (comando == 4) {
			while (d_ang < (M_PI / 4 - DIS_CURVA))//escapar do estado de verificacao dos sensores
			{
				vLeft = -VEL_MOT / 1.5;
				vRight = VEL_MOT / 1.5;
				simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
				simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
				simxGetObjectOrientation(clientID, bubbleRob, -1, angle, simx_opmode_buffer);
				ang = (angle[2]);
				d_ang = abs(ang - ang_inicial);
				//se houve salto da atan, ignora colocando o salto na posicao normal. Como abs, mas funciona melhor
				if (d_ang > (M_PI / 2 - D_ANG)) {
					ang = -ang;
					d_ang = abs(ang - ang_inicial);
				}
				extApi_sleepMs(5);
			}
			simxReadVisionSensor(clientID, LM_Vision_sensor, &readingLMVS, &DataLMVS, &auxLMVS, simx_opmode_buffer);
			simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS, simx_opmode_buffer);
			readingLMVS = DataLMVS[10] < MAX_INTE;
			readingMVS = DataMVS[10] < MAX_INTE;
			while ((readingMVS))//verificar se ta no mesmo estado, tecnicamente inutil devido a funcao acima
			{
				vLeft = -VEL_MOT / 1.5;
				vRight = VEL_MOT / 1.5;
				simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
				simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
				simxReadVisionSensor(clientID, LM_Vision_sensor, &readingLMVS, &DataLMVS, &auxLMVS, simx_opmode_buffer);
				simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
									 simx_opmode_buffer);
				readingLMVS = DataLMVS[10] < MAX_INTE;
				readingMVS = DataMVS[10] < MAX_INTE;
				extApi_sleepMs(5);
			}
			while (!(readingMVS))//gira ate os sensores captarem a linha
			{
				vLeft = -VEL_MOT / 1.5;
				vRight = VEL_MOT / 1.5;
				simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
				simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
				simxReadVisionSensor(clientID, LM_Vision_sensor, &readingLMVS, &DataLMVS, &auxLMVS, simx_opmode_buffer);
				simxReadVisionSensor(clientID, Middle_Vision_sensor, &readingMVS, &DataMVS, &auxMVS,
									 simx_opmode_buffer);
				readingLMVS = DataLMVS[10] < MAX_INTE;
				readingMVS = DataMVS[10] < MAX_INTE;
				extApi_sleepMs(5);
			}
		}
		//zera variaveis e manda o robo parar
		X_inicial = 0;
		Y_inicial = 0;
		dx = 0;
		dy = 0;
		d_ang = 0;
		ang = 0;
		vLeft = 0;
		vRight = 0;
		comando = 0;
		simxSetJointTargetVelocity(clientID, leftMotorHandle, (simxFloat) vLeft, simx_opmode_streaming);
		simxSetJointTargetVelocity(clientID, rightMotorHandle, (simxFloat) vRight, simx_opmode_streaming);
		extApi_sleepMs(5);
	}

	shared_memory_object::remove(NOME_DA_MEMORIA2);//destroi memoria
	simxFinish(clientID); // fechando conexao com o servidor
	cout << "Conexao e Servidor fechados!" << std::endl;
	return 0;
}

void set_commands(const vector<int> &c)
{
	commands = c;
	current_command = 0;
}

int get_next_command()
{
	if (current_command < (int) commands.size()) {
		return commands[current_command++];
	}
	return -1;
}