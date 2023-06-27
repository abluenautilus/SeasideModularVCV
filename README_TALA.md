## Tala Module v2.5.2

Tala is a sample-based tabla machine for VCV Rack. The tabla is a two-piece percussive instrument set used in Indian classical music. Given the tradition of long hours of practice, melodic instrumentalists often use an electronic tabla machine as accompaniment. This module attempts to replicate a tabla machine more than it intends to simulate a live tabla player. 

**Bols**

Tabla players refer to each of the many sounds the instrument can make with a special word called a "bol". Tala can produce 12 individual bols, corresponding to the 12 buttons and jacks on the top half of the instrument. You can trigger individual bols either my manually pressing the button, or sending a trigger into the corresponding input jack.  

*Sample mode* Each bol has several samples associated with it. Sample mode determines how those samples are chosen when a bol is triggered. 

• Mode 0: Only a single, basic sample will always be played for a given bol
• Mode 1: Tala will cycle through the samples for a given bol. Every time it is triggered, the next sample will be played. 
• Mode 2: Tala will randomly choose a sample each time a bol is triggered. 

**Thekas**

Tala can also reproduce traditional drum patterns known as thekas. Each theka consists of a sequence of bols of a specific length, as well as a series of accents. For instance, one of the most common thekas, "teen tal", is a 16 bol sequence with accents on the 1st, 5th, and 13th beats. 

You can select a theka using the up and down arrow buttons. Each trigger coming in to the CLOCK input plays the next beat of the sequence. When tala is playing a theka, the blue play light (PLY) lighs up. Accented notes activate the white ACC light. The first note of the rhythm cycle is known as "sam" and activates the green SAM light. 

The input jack to the right of the theka selection buttons will select a random theka when a trigger is detected. 

The accent effect can be replicated when not playing a theka by sending a trigger or gate into the ACC input. Any bols triggeres while ACC is on will be accented. 

Notes:

- Some thekas include compound bols that are not currently available to play separately (e.g. "TiRaKiTa".) 

- You can send additional triggers to the bols while a theka is playing. However, the accent input is disabled while the Playing light is on, and accents are controlled only by the stored sequence. 

-  Tala supports sample rates up to 192kHz. Higher than that you will get silence. Above 92kHz performance is currently somewhat unreliable. Who in the world is running at these sample rates?!
