/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015 Christian Kreuzberger and Daniel Posch, Alpen-Adria-University
 * Klagenfurt
 *
 * This file is part of amus-httpSIM, based on httpSIM. See AUTHORS for complete list of
 * authors and contributors.
 *
 * amus-httpSIM and httpSIM are free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * amus-httpSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * amus-httpSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef HTTP_MULTIMEDIACONSUMER_H
#define HTTP_MULTIMEDIACONSUMER_H

#include "http-client.h"



#include "ns3/random-variable-stream.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"


#include "ns3/traced-callback.h"
#include "ns3/ptr.h"
#include "ns3/simple-ref-count.h"

#include "ns3/point-to-point-module.h"

#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"

#include "libdash.h"

#include "multimedia-player.h"


#define MULTIMEDIA_CONSUMER_LOOP_TIMER 0.1
#define MIN_BUFFER_LEVEL 4.0


using namespace dash::mpd;


namespace ns3 {




/**
 * @ingroup http-apps
 * @brief http application for sending out Interest packets at a "constant" rate (Poisson process)
 */
template<class Parent>
class MultimediaConsumer : public Parent {
  typedef Parent super;
public:
  enum DownloadType { MPD = 0, InitSegment = 1, Segment = 2 };
  static TypeId
  GetTypeId();

  /**
   * \brief Default constructor
   * Sets up randomizer function and packet sequence number
   */
  MultimediaConsumer();
  virtual ~MultimediaConsumer();

  virtual void
  StartApplication();

  virtual void
  StopApplication();

  virtual std::string
  getMpdUrl ();

  virtual void
  setMpdUrl (std::string url);


protected:
  virtual void
  OnFileReceived(unsigned status, unsigned length);

  bool
  DecompressFile ( std::string source, std::string filename );

  std::string m_mpdUrl;     ///< \brief http URL of the MPD
  unsigned int m_screenWidth; ///< \brief The spatial width of the simulated screen
  unsigned int m_screenHeight; ///< \brief The spatial height of the simulated screen
  std::string m_deviceType;   ///< \brief The device type
  bool m_allowUpscale;        ///< \brief Whether or not it is possible to upscale content with lower resolutions to the screen width/height
  bool m_allowDownscale;      ///< \brief Whether or not it is possible to downscale content with higher resolutions to the screen width/height
  unsigned int m_maxBufferedSeconds; ///< \brief The maximum amount of buffered seconds
  double startupDelay;

  std::string m_startRepresentationId;  ///< \brief The representation ID for initializing streaming
  std::string m_adaptationLogicStr;     ///< \brief The adaptation logic that should be used


  std::string m_tempDir; ///< \brief a temporary directory for storing and parsing the downloaded mpd file
  std::string m_tempMpdFile; ///< \brief path to the temporary MPD file


  dash::mpd::IMPD *mpd; ///< \brief Pointer to the MPD
  dash::player::MultimediaPlayer *mPlayer;

  std::map<std::string, IRepresentation*> m_availableRepresentations; ///< \brief a map with available representations
  std::string m_baseURL; ///< \brief the base URL as extracted from the MPD
  std::string m_initSegment; ///< \brief the URI of the init segment
  std::string m_curRepId; ///< \brief the representation ID that's currently being downloaded

  bool m_isLayeredContent; ///< \brief tells us whether the content that this player is requesting is layered (e.g., SVC) or not (e.g., AVC)

  std::vector<std::string /* representation_id */> m_downloadedRepresentations;
  int64_t m_startTime;

  int64_t m_freezeStartTime;

  uint32_t m_userId;
  uint32_t m_videoId;

  static std::string alphabet;

  bool m_mpdParsed;
  bool m_initSegmentIsGlobal;
  bool m_hasInitSegment;
  bool m_hasStartedPlaying;
  bool m_hasDownloadedAllSegments;
  bool traceNotDownloadedSegments;
  unsigned int totalConsumedSegments;

  dash::mpd::ISegmentURL* requestedSegmentURL;
  const dash::mpd::IRepresentation* requestedRepresentation;
  unsigned int requestedSegmentNr;



  void SchedulePlay(double wait_time = MULTIMEDIA_CONSUMER_LOOP_TIMER);
  void DoPlay();
  double consume();

  EventId m_consumerLoopTimer;
  EventId m_downloadEventTimer;

  std::vector<std::string> m_downloadedInitSegments; ///< \brief a vector containing the representation IDs of which we have init segments
  DownloadType m_currentDownloadType;


  virtual void
  OnMpdFile();

  virtual void
  OnMultimediaFile();

  virtual void
  ScheduleDownloadOfInitSegment();

  virtual void
  DownloadInitSegment();

  virtual void
  ScheduleDownloadOfSegment();

  virtual void
  DownloadSegment();

  //Vitalii: removed dependencyIDs to fit the videoID. Dunno how to make more than 8 params in a traced callback :)
  TracedCallback<Ptr<ns3::Application> /*App*/, unsigned int /* UserId */, unsigned int /* videoId */, unsigned int /*SegmentNr*/,
                std::string /*RepresentationId*/, unsigned int /* experiendedBitrate */,
                unsigned int /*StallingTime*/, unsigned int /* buffer level *//*,
                std::vector<std::string> DependencyIds*/> m_playerTracer;



};



} // namespace ns3

#endif
